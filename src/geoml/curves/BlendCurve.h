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
#include "geoml/error.h"

#include <cmath>

#include "geometry/Continuity.h"

#include <BRep_Tool.hxx>

namespace geoml 
{

class BlendCurveConnection
{
    Continuity ct = Continuity::G1;

    BlendCurveConnection(TopoDS_Edge const& edge, gp_Pnt const& near_connection_point, Continuity continuity, Standard_Real form_factor_1, Standard_Real form_factor_2, bool outward_direction = true)
    : m_near_connection_point(near_connection_point)
    , m_continuity(continuity)
    , m_beta0(form_factor_1)
    , m_beta1(form_factor_2)
    , m_outward_direction(outward_direction)
    {
	    m_curve = BRep_Tool::Curve(edge, m_curve_first_param, m_curve_last_param); 
	    compute_blend_parameter();
    } 

    Handle(Geom_Curve) m_curve; // in der aktuellen Implementierung werden Geom_BSpline curves verwendet.
    gp_Pnt m_near_connection_point;
    Continuity m_continuity;
    Standard_Real m_beta0;
    Standard_Real m_beta1;
    bool m_outward_direction;
    Standard_Real m_curve_first_param;
    Standard_Real m_curve_last_param;
    Standard_Real m_curve_blend_param;

private:
    void compute_blend_parameter()
    {
        Standard_Real distance_first_point_on_curve_to_near_connection_point = m_curve->Value(m_curve_first_param).SquareDistance(m_near_connection_point);  

	    Standard_Real distance_last_point_on_curve_to_near_connection_point = m_curve->Value(m_curve_last_param).SquareDistance(m_near_connection_point);

    	if (std::abs(distance_first_point_on_curve_to_near_connection_point - distance_last_point_on_curve_to_near_connection_point) < 1e-5)
    	{ 
	        throw geoml::Error("near_connection_point has the same distance to both vertices of the edge");
        }
        else if (distance_first_point_on_curve_to_near_connection_point < distance_last_point_on_curve_to_near_connection_point)
        {
            m_curve_blend_param = m_curve_first_param;
        }
        else 
        {
            m_curve_blend_param = m_curve_last_param;
        }
    }

};

} // namespace geoml