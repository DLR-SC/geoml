#pragma once

/**
 * @brief geoml/utility/modeling.h includes general functions useful for geometric modeling.
 */

#include <geoml/geoml.h>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Vec.hxx>
#include <vector>
#include <TopoDS_Shape.hxx> 
#include <TopoDS_Face.hxx> 
#include <TopoDS.hxx> 
#include <TopoDS_Solid.hxx> 
#include <initializer_list> 
#include "Geom_Curve.hxx"
#include <Geom_TrimmedCurve.hxx>


namespace geoml
{

class Transformation;

/**
 * @brief Extracts the control points of a Geom_BSplineSurface
 * 
 * @param b_spline_surface A Geom_BSplineSurface
 */
GEOML_API_EXPORT TColgp_Array2OfPnt // write unit tests
extract_control_points_surface(const Handle(Geom_BSplineSurface)& b_spline_surface);

/**
 * @brief Extracts a control point colums (U direction) or a control point row (V direction)
 *        of a Geom_BSplineSurface
 * 
 * @param b_spline_surface A Geom_BSplineSurface
 * @param UV_direction 1 for extracting the control points in U-direction (columns), 2 for the 
 *                     V-direction (rows)
 * @param index The index of the control point column or row         
 */
GEOML_API_EXPORT TColgp_Array1OfPnt  
extract_control_point_column_row (const Handle(Geom_BSplineSurface)& b_spline_surface, 
                                  int UV_direction, int index);

/**
 * @brief Move an array of points in vector direction of vector dir times a pre-factor
 * 
 * @param points A 1-d collection of points to move
 * @param direction The direction, in which the movement should take place
 * @param factor The pre-factor of the movement         
 */
GEOML_API_EXPORT TColgp_Array1OfPnt 
move (const TColgp_Array1OfPnt& points, gp_Vec direction, double factor);

/**
 * @brief Move a point in vector direction of vector dir times a pre-factor
 * 
 * @param point The point to move
 * @param direction The direction, in which the movement should take place
 * @param factor The pre-factor of the movement         
 */
GEOML_API_EXPORT gp_Pnt 
move (const gp_Pnt& point, gp_Vec direction, double factor);

/**
 * @brief Move an array of points in vector direction of vector dir times a pre-factor
 * 
 * @param points The 1-d collection of points to move
 * @param direction The direction, in which the movement should take place
 * @param factor The pre-factor of the movement         
 */
GEOML_API_EXPORT Handle(TColgp_HArray1OfPnt) 
move (const Handle(TColgp_HArray1OfPnt)& points, gp_Vec direction, double factor);

/**
 * @brief Move an array of points in vector direction of vector dir times a pre-factor
 * 
 * @param points The 1-d collection of points to move
 * @param direction The direction, in which the movement should take place
 * @param factor The pre-factor of the movement         
 */
GEOML_API_EXPORT std::vector<gp_Pnt> 
move (const std::vector<gp_Pnt> & points, gp_Vec direction, double factor);

/**
 * @brief Creates a translated copy of a shape. The translation is performed along a
 * given direction and factor
 * 
 * @param origin The shape to be translated
 * @param direction The direction of translation
 * @param factor The factor applied to the direction for the translation (default: 1.0)
 * @return TopoDS_Shape 
 */
TopoDS_Shape moved(TopoDS_Shape const& origin, gp_Vec const& direction, double factor = 1.0);

/**
 * @brief Creates repeated copies of an input shape by repeatedly transforming the shape 
 * n times given a geoml::Transformation.
 * 
 * @param origin The shape to be repeated n times
 * @param translation The transformation to be applied to each copy, relative to the previous copy
 * @param n_repeats The number of repetitions
 * @return TopoDS_Compound A compound holding all resulting copies
 */
TopoDS_Compound repeat_shape(TopoDS_Shape const& origin, Transformation const& trans, int n_repeats);

/**
 * @brief Create a TColgp_Array2OfPnt array out of a vector of TColgp_Array1OfPnt arrays
 *        of equal size 
 * @param point_lists Vector of point arrays of equal length
 * @param rows_columns 1 if the point arrays should be the rows, 2 if they should be the
 *                     columns       
 */
GEOML_API_EXPORT TColgp_Array2OfPnt 
create_point_net_from_arrays (const std::vector<TColgp_Array1OfPnt>& point_lists,
                              int rows_columns);

/**
 * @brief Add a fillet to an edge 
 * @param solid_shape The solid with the edge to be filleted
 * @param edge_index The index of the edge to fillet
 * @param radius The radius of the fillet    
 */
GEOML_API_EXPORT TopoDS_Shape
make_fillet (const TopoDS_Shape &solid_shape , int edge_index, double radius);

/**
 * @brief Add fillets to an edge 
 * @param solid_shape The solid with the edge to be filleted
 * @param edge_indices The indices of the edges to fillet
 * @param radius The radius of the fillet    
 */
GEOML_API_EXPORT TopoDS_Shape
make_fillet (const TopoDS_Shape &solid_shape , const std::vector<int> &edge_indices, double radius);

/**
 * @brief Scale a vector 
 * @param vector The vector to scale
 * @param factor  The scale factor 
 */
GEOML_API_EXPORT gp_Vec
scale_vector (const gp_Vec &vector , double factor);

/////////////////////////////////// temporary quick fix for milestone

void writeToStp(Handle_Geom_Curve cur, std::string const& name_file);

void writeGeomEntityToStepFile(Handle_Geom_Surface surface, std::string fileName);

void writeGeomEntityToStepFile(Handle_Geom_BSplineSurface surface, std::string fileName);

int writeGeomEntityToStepFile(const TopoDS_Shape& my_shape, std::string fileName);

//////////////////////////////////////

TopoDS_Face make_face (const Handle_Geom_BSplineSurface & surface);

int test_func (const std::vector< std::vector<gp_Pnt> > & control_points);

Handle(Geom_Curve) convert_to_geom_curve(const Handle(Geom_TrimmedCurve)& trimmed_curve);
    
Handle(Geom_BSplineCurve) geom_convert_from_trimmed_curve(const Handle(Geom_TrimmedCurve)& trimmed_curve);

gp_Pnt vector_of_points_at_index (const std::vector<gp_Pnt> & points, int index);

std::vector<Handle(Geom_Curve)> vector_b_spline_to_geom_curve_converter (const std::vector<Handle(Geom_BSplineCurve)> input_vec);

Handle(Geom_Curve) convert_to_geom_curve(const Handle(Geom_BSplineCurve)& bspline_curve);

Handle(Geom_Surface) convert_to_geom_surface(const Handle(Geom_BSplineSurface)& bspline_surface);

TopoDS_Shape fuse_two_solids (const TopoDS_Solid & solid_1, const TopoDS_Solid & solid_2);

TopoDS_Shape remove_faces_from_shape (const TopoDS_Shape & shape, const std::vector<int> & indices_of_faces_to_remove);

TopoDS_Shape extrude_surface (const Handle(Geom_BSplineSurface) & surface, const gp_Vec & extrusion_vector);

} // namespace geoml