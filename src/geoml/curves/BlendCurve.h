/* 
* Copyright (C) 2024 German Aerospace Center (DLR/SC)
*
* Created: 2025-01-12 Anton Reiswich <Anton.Reiswich@dlr.de>
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
/**
* @file
* @brief  Implementation of a blend curve.
*/
#pragma once

#include <geoml/geoml.h>  

#include "geometry/Continuity.h"
#include "geoml/data_structures/conversions.h"
#include "geoml/geom_topo_conversions/geom_topo_conversions.h"

#include <TColgp_Array1OfPnt.hxx>

#include <TopoDS_Edge.hxx>
#include <Geom_Curve.hxx>
#include <gp_Vec.hxx>
#include <Geom_BezierCurve.hxx>

#include <vector>

namespace geoml 
{

/**
 * @brief The BlendCurveConnection struct contains the relevant information a blend curve needs to 
 * to connect to a given curve.
 *
 * It contains the curve to be blended, the required continuity, the beta and gamma factor to control the shape of the
 * blend curve, a toggle whether or not the blend curve should be inward or outward facing relative to the curve to blend, 
 * and the parameters of the curves start and end point, as well as the parameter of the blend point.
 */
struct BlendCurveConnection
{
    GEOML_API_EXPORT BlendCurveConnection(TopoDS_Edge const& edge, gp_Pnt const& near_connection_point, GContinuity continuity, bool outward_direction = true, Standard_Real beta = 1., Standard_Real gamma = 1.);

    Handle(Geom_Curve) m_curve;
    gp_Pnt m_near_connection_point;
    GContinuity m_continuity;
    Standard_Real m_beta;
    Standard_Real m_gamma;
    bool m_outward_direction;
    Standard_Real m_curve_first_param;
    Standard_Real m_curve_last_param;
    Standard_Real m_curve_blend_param;

private:
    GEOML_API_EXPORT void compute_blend_parameter();
};

/**
 * @brief The class BlendCurve takes two BlendCurveConnections and computes the blend curve according to the given specifications.
 *
 * Its public API exposes a method that reutrns the required blend curve.
 */
class BlendCurve
{
public:
    
    /**
     * @brief Constructs a BlendCurve object from two BlendCurveConnections.
     * 
     * @param start The BlendCurveConnection specifying the start of the blend curve
     * @param end The BlendCurveConnection specifying the end of the blend curve
     */
    GEOML_API_EXPORT BlendCurve(BlendCurveConnection const& start, BlendCurveConnection const& end);

    /**
     * @brief Returns the blend curve satisfying the required boundary conditions.
     */
    GEOML_API_EXPORT TopoDS_Edge blend_curve();

private:

    GEOML_API_EXPORT void compute_blend_points_and_derivatives_of_start_and_end_curve();

    GEOML_API_EXPORT gp_Pnt formula_for_second_control_point_in_parameter_direction(gp_Pnt first_point, Standard_Real beta, gp_Vec first_derivative);
    GEOML_API_EXPORT gp_Pnt formula_for_second_control_point_against_parameter_direction(gp_Pnt first_point, Standard_Real beta, gp_Vec first_derivative);

    GEOML_API_EXPORT gp_Pnt formula_for_third_control_point_in_parameter_direction(gp_Pnt first_point, Standard_Real beta, Standard_Real gamma, gp_Vec first_derivative, gp_Vec second_derivative);
    GEOML_API_EXPORT gp_Pnt formula_for_third_control_point_against_parameter_direction(gp_Pnt first_point, Standard_Real beta, Standard_Real gamma, gp_Vec first_derivative, gp_Vec second_derivative);


    GEOML_API_EXPORT gp_Pnt compute_first_control_point_at_start();
    GEOML_API_EXPORT gp_Pnt compute_first_control_point_at_end();
    
    GEOML_API_EXPORT gp_Pnt compute_second_control_point_at_start();
    GEOML_API_EXPORT gp_Pnt compute_second_control_point_at_end();

    GEOML_API_EXPORT gp_Pnt compute_third_control_point_at_start();
    GEOML_API_EXPORT gp_Pnt compute_third_control_point_at_end();

    GEOML_API_EXPORT gp_Pnt get_i_th_control_point(unsigned int i, GContinuity contin_start, GContinuity contin_end);

    GEOML_API_EXPORT std::vector<gp_Pnt> control_points_blend_curve();


    BlendCurveConnection m_start;
    BlendCurveConnection m_end;
    
    std::vector<gp_Pnt> m_control_point_vector;

    Standard_Real m_degree;

    gp_Pnt m_blend_point_start;
    gp_Pnt m_blend_point_end;

    gp_Vec m_first_derivative_start_curve;
    gp_Vec m_second_derivative_start_curve;
    
    gp_Vec m_first_derivative_end_curve;
    gp_Vec m_second_derivative_end_curve;

};

/**
 * @brief The free function blend curve returns a blend curve according to the specifications in the BlendCurveConnections of 
 * its arguments.
 */
GEOML_API_EXPORT TopoDS_Edge blend_curve(BlendCurveConnection const& start, BlendCurveConnection const& end);

} // namespace geoml