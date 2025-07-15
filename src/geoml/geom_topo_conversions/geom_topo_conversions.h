#pragma once
/**
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * @brief geoml/geom_topo_conversions/geom_topo_conversions.h includes functions to convert between OCCT's geometry and topology data types
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
 * @param tolerance Sets the tolerance for resolution of degenerate edges
 */
GEOML_API_EXPORT TopoDS_Face SurfaceToFace(const Handle(Geom_Surface) &surface, Standard_Real tolDegEd);

/**
 * @brief Converts a Geom_Curve in a TopoDS_Edge
 * 
 * 
 * @param curve A Handle to a Geom_Curve
 */
GEOML_API_EXPORT TopoDS_Edge CurveToEdge(const Handle(Geom_Curve) &curve);

/**
 * @brief Extracts the Geom_Curve of a TopoDS_Edge
 * 
 * 
 * @param edge A TopoDS_Edge
 */
GEOML_API_EXPORT Handle(Geom_Curve) EdgeToCurve(TopoDS_Edge const& edge);

} // namespace geoml
