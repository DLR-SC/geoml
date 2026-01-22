#include "surfaces/surfaces.h"
#include "geoml/curves/curves.h"
#include "geoml/data_structures/conversions.h"

#include "geometry/curve-networks/InterpolateCurveNetwork.h"
#include "geometry/CurvesToSurface.h"
#include "common/CommonFunctions.h"

#include "BRepBuilderAPI_MakeEdge.hxx"
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx> //
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
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
                                 Standard_Real tolerance)
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
    return new Geom_BSplineSurface(geoml::Array2d_to_TCol(control_points),
                                                                    geoml::Array2d_to_TCol(weights),
                                                                    geoml::StdVector_to_TCol(U_knots),
                                                                    geoml::StdVector_to_TCol(V_knots),
                                                                    geoml::StdVector_to_TCol(U_mults),
                                                                    geoml::StdVector_to_TCol(V_mults),
                                                                    U_degree,
                                                                    V_degree,
                                                                    U_periodic,
                                                                    V_periodic);

}

Handle(Geom_Surface)
create_surface(const gp_Pnt &p_1, const gp_Pnt &p_2, const gp_Pnt &p_3, const gp_Pnt &p_4)
{
    TopoDS_Face face = create_face(p_1, p_2, p_3, p_4);
    Handle(Geom_Surface) surface = BRep_Tool::Surface(face);

    return surface;
} 

TopoDS_Face
create_face(const gp_Pnt &p_1, const gp_Pnt &p_2, const gp_Pnt &p_3, const gp_Pnt &p_4)
{
    TopoDS_Face face = BuildFace(p_1, p_2, p_4, p_3);

    return face;
}

} // namespace geoml
