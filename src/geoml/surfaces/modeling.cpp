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
