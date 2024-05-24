#include "surfaces/modeling.h"
#include "geoml/curves/modeling.h"

#include "geometry/curve-networks/InterpolateCurveNetwork.h"
#include "geometry/CurvesToSurface.h"

#include "BRepBuilderAPI_MakeEdge.hxx"
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <GeomConvert.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_Array1OfReal.hxx>

namespace geoml{

Handle(Geom_BSplineSurface) interpolate_curve_network(const std::vector<Handle (Geom_Curve)> &ucurves,
                                 const std::vector<Handle (Geom_Curve)> &vcurves,
                                 double tolerance)
{
    InterpolateCurveNetwork interpolator(ucurves, vcurves, tolerance);

    return interpolator.Surface();
}

Handle(Geom_BSplineSurface)
interpolate_curves(const std::vector<Handle (Geom_Curve)> &ucurves, unsigned int max_degree,
                          bool join_continuously)
{
    CurvesToSurface c2s(ucurves, join_continuously);
    c2s.SetMaxDegree(max_degree);
    return c2s.Surface();
}

TopoDS_Shape
revolving_shape(const TopoDS_Shape& profile_shape,
                const gp_Pnt& start_point_rotation_axis,
                const gp_Vec& rotation_axis_direction,
                const Standard_Real angle)
{   
    gp_Dir dir (rotation_axis_direction);
    gp_Ax1 axis (start_point_rotation_axis, dir);
    
    BRepPrimAPI_MakeRevol revol_maker(profile_shape, axis, angle); 

    TopoDS_Shape revol_shape = revol_maker.Shape();

    return revol_shape;
}

Handle(Geom_BSplineSurface)
nurbs_surface(const Array2d<gp_Pnt> &control_points, 
                 const Array2d<Standard_Real> &weights, 
                 const std::vector<Standard_Real> &U_knots, 
                 const std::vector<Standard_Real> &V_knots, 
                 const std::vector<int> &U_mults, 
                 const std::vector<int> &V_mults, 
                 const int U_degree, 
                 const int V_degree, 
                 const bool U_periodic,
                 const bool V_periodic)
{
    TColgp_Array2OfPnt control_points_col (1, control_points.rowLength(), 1, control_points.colLength());

    for(int i = 0; i < control_points.rowLength(); ++i)
    {
        for(int j = 0; j < control_points.colLength(); ++j)
        {
            control_points_col.SetValue(i + 1, j + 1, control_points.at(i, j));
        }
    }

    TColStd_Array2OfReal weights_col (1, control_points.rowLength(), 1, control_points.colLength());

    for(int i = 0; i < control_points.rowLength(); ++i)
    {
        for(int j = 0; j < control_points.colLength(); ++j)
        {
            weights_col.SetValue(i + 1, j + 1, weights.at(i, j));
        }
    }
 
    TColStd_Array1OfReal U_knots_col (1, U_knots.size());

    for(int i = 0; i < U_knots.size(); ++i)
    {
        U_knots_col.SetValue(i + 1, U_knots.at(i));
    }

    TColStd_Array1OfReal V_knots_col (1, V_knots.size());

    for(int i = 0; i < V_knots.size(); ++i)
    {
        V_knots_col.SetValue(i + 1, V_knots.at(i));
    }

    TColStd_Array1OfInteger U_mults_col (1, U_mults.size());

    for(int i = 0; i < U_mults.size(); ++i)
    {
        U_mults_col.SetValue(i + 1, U_mults.at(i));
    }

    TColStd_Array1OfInteger V_mults_col (1, V_mults.size());

    for(int i = 0; i < V_mults.size(); ++i)
    {
        V_mults_col.SetValue(i + 1, V_mults.at(i));
    }

    Handle(Geom_BSplineSurface) srf_handle = new Geom_BSplineSurface( control_points_col,
                                                                    weights_col,
                                                                    U_knots_col,
                                                                    V_knots_col,
                                                                    U_mults_col,
                                                                    V_mults_col,
                                                                    U_degree,
                                                                    V_degree,
                                                                    U_periodic,
                                                                    V_periodic );

    return srf_handle; 
}

} // namespace geoml
