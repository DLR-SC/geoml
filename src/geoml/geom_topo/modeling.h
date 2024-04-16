#pragma once

/**
 * @brief geoml/geom_topo/modeling.h includes functions to convert between OCCT's geometry and topology data types
 */
#include <geoml/geoml.h>

#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>

class Geom_Surface;
class Geom_Curve;

namespace geoml
{

/**
 * @brief Converts a Geom_Surface into a TopoDS_Face
 * 
 * 
 * @param surface A Handle to a Geom_Surface
 */
GEOML_API_EXPORT TopoDS_Face
SurfaceToFace(const Handle(Geom_Surface) &surface);

/**
 * @brief Converts a Geom_Curve in a TopoDS_Edge
 * 
 * 
 * @param curve A Handle to a Geom_Curve
 */
GEOML_API_EXPORT TopoDS_Edge
CurveToEdge(const Handle(Geom_Curve) &curve);

} // namespace geoml
