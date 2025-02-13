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

#include "geoml/Continuity.h"
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
 * @brief The BlendCurveConnection class contains the relevant information a blend curve needs to 
 * to connect to a given edge.
 *
 * It contains the edge to be blended, a choice of the vertex of the edge that defines the attachment point of the blend, the required geometric continuity 
 * in that point, a toggle whether or not the blend curve should be geometrically inward or outward facing 
 * relative to the edge to blend, and finally, the beta and gamma factors to control the shape of the blend curve for higher geometric continuities. 
 * 
 * @param edge The edge to be blended
 * @param near_connection_point This point determines which vertex of the edge the blend curve attaches to.  
 *                              The blend curve attaches to the vertex that is closest to near_connection_point.
 *                              If both vertices have the same distance from near_connection_point, an error is thrown.
 * @param continuity The parameter continuity can take the values geoml::GContinuity::G0, geoml::GContinuity::G1 and geoml::GContinuity::G2 and sets the
 *                   geometric continuity of the blend.
 * @param outward_direction This parameter only plays a role if the continuity is G1 or higher. If true (default value), the blend curve will be 
 *                          geometrically directed inside the edge. If false, the blend curve will be
 *                          geometrically outward directed with respect to the edge. 
 * @param beta This parameter only plays a role if the continuity is G1 or higher. The parameter beta determines the distance of the second control point
 *             of the blend curve from the start point of the blend. This distance is given by beta * distance(second control point of blend, start 
 *             point of blend).
 * @param gamma The parameter gamma only play a role if the continuity if G2. In this case, it determines the distance of the third control point of the blend
 *              curve to the start point of the blend curve, measured in tangential direction at the blend curve. This tangential distance is given by
 *              gamma * 2 * beta * distance(second control point of blend, start point of blend).   
 */
class BlendCurveConnection
{
public:

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
    gp_Pnt m_blend_point;
    gp_Vec m_first_derivative_curve;
    gp_Vec m_second_derivative_curve;

private:

    GEOML_API_EXPORT void compute_blend_parameter();

    GEOML_API_EXPORT void compute_blend_point_and_derivatives();
};

/**
 * @brief The class BlendCurve takes two BlendCurveConnections and computes the blend curve according to the given specifications.
 *
 * Its public API exposes a method that returns the required blend curve.
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

    GEOML_API_EXPORT gp_Pnt control_point_2(BlendCurveConnection &side);
    GEOML_API_EXPORT gp_Pnt control_point_3(BlendCurveConnection &side);

    GEOML_API_EXPORT gp_Pnt compute_control_point(int index, BlendCurveConnection &side);

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