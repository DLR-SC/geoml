#pragma once

/**
 * @brief geoml/geom_topo/modeling.h includes functions to convert between OCCT's geometry and topology data types
 */
#include <geoml/geoml.h>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>



namespace geoml
{

/**
 * @brief Converts a Geom_BSplineSurface in a TopoDS_Shape
 * 
 * 
 * @param surface A Handle to a Geom_BSplineSurface
 */
GEOML_API_EXPORT TopoDS_Face
SurfaceToFace(const Handle(Geom_Surface) &surface);

/**
 * @brief Converts a Geom_BSplineCurve in a TopoDS_Edge
 * 
 * 
 * @param surface A Handle to a Geom_BSplineCurve
 */
GEOML_API_EXPORT TopoDS_Edge
CurveToEdge(const Handle(Geom_Curve) &curve);

} // namespace geoml
