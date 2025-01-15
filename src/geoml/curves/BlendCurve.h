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

struct BlendCurveConnection
{
    BlendCurveConnection(TopoDS_Edge const& edge, gp_Pnt const& near_connection_point, GContinuity continuity, bool outward_direction = true, Standard_Real form_factor_1 = 1., Standard_Real form_factor_2 = 1.);

    Handle(Geom_Curve) m_curve; // in der aktuellen Implementierung werden Geom_BSpline curves verwendet.
    gp_Pnt m_near_connection_point;
    GContinuity m_continuity;
    Standard_Real m_form_factor_1;
    Standard_Real m_form_factor_2;
    bool m_outward_direction;
    Standard_Real m_curve_first_param;
    Standard_Real m_curve_last_param;
    Standard_Real m_curve_blend_param;

private:
    void compute_blend_parameter();
};

class BlendCurve
{
public:
    
    BlendCurve(BlendCurveConnection const& start, BlendCurveConnection const& end);

    TopoDS_Edge blend_curve();

private:

    void compute_blend_points_and_derivatives_of_start_and_end_curve();

    gp_Pnt compute_first_control_point_at_start();
    gp_Pnt compute_second_control_point_at_start();
    gp_Pnt compute_third_control_point_at_start();

    gp_Pnt compute_first_control_point_at_end();
    gp_Pnt compute_second_control_point_at_end();
    gp_Pnt compute_third_control_point_at_end();

    gp_Pnt get_i_th_control_point(unsigned int i, GContinuity contin_start, GContinuity contin_end);

    std::vector<gp_Pnt> control_points_blend_curve();


    BlendCurveConnection m_start;
    BlendCurveConnection m_end;
    
    std::vector<gp_Pnt> m_control_point_vector;
    
    Handle(Geom_Curve) m_oriented_start_curve;
    Handle(Geom_Curve) m_oriented_end_curve;
    
    Standard_Real m_blend_parameter_oriented_start_curve;
    Standard_Real m_blend_parameter_oriented_end_curve;

    gp_Pnt m_blend_point_start;
    gp_Pnt m_blend_point_end;

    gp_Vec m_first_derivative_oriented_start_curve;
    gp_Vec m_second_derivative_oriented_start_curve;
    
    gp_Vec m_first_derivative_oriented_end_curve;
    gp_Vec m_second_derivative_oriented_end_curve;

};

TopoDS_Edge blend_curve(BlendCurveConnection const& start, BlendCurveConnection const& end);

} // namespace geoml