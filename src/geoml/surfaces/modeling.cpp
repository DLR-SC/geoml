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

Handle(Geom_BSplineSurface)
revolving_surface(const Handle(Geom_BSplineCurve)& profile_curve,
                         const gp_Ax1& rotation_axis,
                         const Standard_Real angle)
{   
    BRepBuilderAPI_MakeEdge edgeMaker(profile_curve);
    TopoDS_Edge edge = edgeMaker.Edge();
    TopoDS_Shape shape = edge;
 
    BRepPrimAPI_MakeRevol revol_maker(shape, 
    								  rotation_axis,
                                      angle); 

    TopoDS_Shape revol_shape = revol_maker.Shape();

    // Now, extract a B-spline surface out of the TopoDS_Shape 
    // For that, extract the face first
    TopoDS_Face face;
    for (TopExp_Explorer explorer(revol_shape, TopAbs_FACE); explorer.More(); explorer.Next()) {
        face = TopoDS::Face(explorer.Current());
        break; // Let us assume, there is only one face in the shape
    }

    Handle(Geom_Surface) revol_geom_surface = BRep_Tool::Surface(face);

    Handle(Geom_BSplineSurface)
        revol_b_spline_surface = GeomConvert::SurfaceToBSplineSurface(revol_geom_surface);

    return revol_b_spline_surface;
}

} // namespace geoml
