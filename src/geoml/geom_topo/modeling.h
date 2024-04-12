#pragma once

/**
 * @brief geoml/geom_topo/modeling.h includes functions to convert between OCCT's geometry and topology data types
 */
#include <geoml/geoml.h>
#include <Geom_BSplineSurface.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>



namespace geoml
{

/**
 * @brief Converts a Geom_BSplineSurface in a TopoDS_Shape
 * 
 * 
 * @param surface A Handle to a Geom_BSplineSurface
 */
GEOML_API_EXPORT TopoDS_Face
Geom_BSplineSurfaceToTopoDS_Face(const Handle(Geom_BSplineSurface) &surface);

} // namespace geoml
