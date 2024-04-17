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
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>

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

} // namespace geoml
