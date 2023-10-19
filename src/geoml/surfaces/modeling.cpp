#include "surfaces/modeling.h"

#include "geometry/curve-networks/InterpolateCurveNetwork.h"
#include "geometry/CurvesToSurface.h"

// here, I have to clean up and delete the includes that are not used:
#include <iostream>
#include <cmath>
#include <vector>
#include <geoml/curves/modeling.h>
#include <gp_Pnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include "Geom_Curve.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRepBuilderAPI_MakeFace.hxx"
#include <TopoDS_Shape.hxx>
#include "STEPControl_Writer.hxx"
#include <GeomAPI_PointsToBSpline.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <GC_MakeSegment.hxx>
#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"
#include <Geom_Circle.hxx>
#include <gp_Ax2.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <gp_Circ.hxx>
#include <GeomConvert.hxx>
#include <NCollection_Array2.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <GeomConvert.hxx>



Handle(Geom_BSplineSurface)
geoml::interpolate_curve_network(const std::vector<Handle (Geom_Curve)> &ucurves,
                                 const std::vector<Handle (Geom_Curve)> &vcurves,
                                 double tolerance)
{
    geoml::InterpolateCurveNetwork interpolator(ucurves, vcurves, tolerance);

    return interpolator.Surface();
}

Handle(Geom_BSplineSurface)
geoml::interpolate_curves(const std::vector<Handle (Geom_Curve)> &ucurves, unsigned int max_degree,
                          bool join_continously)
{
    geoml::CurvesToSurface c2s(ucurves, join_continously);
    c2s.SetMaxDegree(max_degree);
    return c2s.Surface();
}

Handle(Geom_BSplineSurface)
geoml::revolving_surface(const Handle(Geom_BSplineCurve)& profile_curve,
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

Handle(Geom_BSplineSurface)
geoml::b_spline_surface(const TColgp_Array2OfPnt &control_points, 
                 const TColStd_Array2OfReal &weights, 
                 const TColStd_Array1OfReal &U_knots, 
                 const TColStd_Array1OfReal &V_knots, 
                 const TColStd_Array1OfInteger &U_mults, 
                 const TColStd_Array1OfInteger &V_mults, 
                 const Standard_Integer U_degree, 
                 const Standard_Integer V_degree, 
                 const Standard_Boolean U_periodic, 
                 const Standard_Boolean V_periodic)
{
    Handle(Geom_BSplineSurface) srf_handle = new Geom_BSplineSurface( control_points,
                                                                    weights,
                                                                    U_knots,
                                                                    V_knots,
                                                                    U_mults,
                                                                    V_mults,
                                                                    U_degree,
                                                                    V_degree,
                                                                    U_periodic,
                                                                    V_periodic );

    return srf_handle;                                        
}                 

