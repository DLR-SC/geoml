#include "curves/BlendCurve.h"

#include "geoml/error.h"

#include <cmath>

#include <BRep_Tool.hxx>

namespace geoml{

BlendCurveConnection::BlendCurveConnection(TopoDS_Edge const& edge, gp_Pnt const& near_connection_point, GContinuity continuity, Standard_Real form_factor_1, Standard_Real form_factor_2, bool outward_direction)
: m_near_connection_point(near_connection_point)
, m_continuity(continuity)
, m_form_factor_1(form_factor_1)
, m_form_factor_2(form_factor_2)
, m_outward_direction(outward_direction)
{
    m_curve = BRep_Tool::Curve(edge, m_curve_first_param, m_curve_last_param); 
    compute_blend_parameter();
} 

void BlendCurveConnection::compute_blend_parameter()
{
    Standard_Real distance_first_point_on_curve_to_near_connection_point = m_curve->Value(m_curve_first_param).SquareDistance(m_near_connection_point);  

    Standard_Real distance_last_point_on_curve_to_near_connection_point = m_curve->Value(m_curve_last_param).SquareDistance(m_near_connection_point);

    if (std::abs(distance_first_point_on_curve_to_near_connection_point - distance_last_point_on_curve_to_near_connection_point) < 1e-5)
    { 
        throw geoml::Error("near_connection_point has the same distance to both vertices of the edge", GENERIC_ERROR);
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

BlendCurve::BlendCurve(BlendCurveConnection const& start, BlendCurveConnection const& end)
    : m_start(start)
    , m_end(end)
{ 
    if (m_start.m_outward_direction == true)
    { 
        if (m_start.m_curve_blend_param == m_start.m_curve_last_param)
        {   
            m_oriented_start_curve = m_start.m_curve;
            m_blend_parameter_oriented_start_curve = m_start.m_curve_blend_param;
        }
        if (m_start.m_curve_blend_param == m_start.m_curve_first_param)
        {
            m_oriented_start_curve = m_start.m_curve -> Reversed();
            m_blend_parameter_oriented_start_curve = m_start.m_curve -> ReversedParameter(m_start.m_curve_blend_param);
        }
    }
    else
    {
        if (m_start.m_curve_blend_param == m_start.m_curve_last_param)
        {
            m_oriented_start_curve = m_start.m_curve -> Reversed();
            m_blend_parameter_oriented_start_curve = m_start.m_curve -> ReversedParameter(m_start.m_curve_blend_param);
        }
        if (m_start.m_curve_blend_param == m_start.m_curve_first_param)
        {
            m_oriented_start_curve = m_start.m_curve;   
            m_blend_parameter_oriented_start_curve = m_start.m_curve_blend_param;
        }
    }

    if (m_end.m_outward_direction == true )
    { 
        if (m_end.m_curve_blend_param == m_end.m_curve_first_param)
        {
            m_oriented_end_curve = m_end.m_curve;
            m_blend_parameter_oriented_end_curve = m_end.m_curve_blend_param;
        }
        if (m_end.m_curve_blend_param == m_end.m_curve_last_param)
        {
            m_oriented_end_curve = m_end.m_curve -> Reversed();
            m_blend_parameter_oriented_end_curve = m_start.m_curve -> ReversedParameter(m_start.m_curve_blend_param);
        }
    }
    else
    {
        if (m_end.m_curve_blend_param == m_end.m_curve_first_param)
        {
            m_oriented_end_curve = m_end.m_curve -> Reversed();
            m_blend_parameter_oriented_end_curve = m_start.m_curve -> ReversedParameter(m_start.m_curve_blend_param);
        }
        if (m_end.m_curve_blend_param == m_end.m_curve_last_param)
        {
            m_oriented_end_curve = m_end.m_curve;  
            m_blend_parameter_oriented_end_curve = m_end.m_curve_blend_param;
        }
    }

    compute_blend_points_and_derivatives_of_start_and_end_curve();

}

void BlendCurve::compute_blend_points_and_derivatives_of_start_and_end_curve()
{
    m_oriented_start_curve->D2(m_blend_parameter_oriented_start_curve, m_blend_point_start, m_first_derivative_oriented_start_curve, m_second_derivative_oriented_start_curve);
    m_oriented_end_curve->D2(m_blend_parameter_oriented_end_curve, m_blend_point_end, m_first_derivative_oriented_end_curve, m_second_derivative_oriented_end_curve);
}

gp_Pnt BlendCurve::compute_first_control_point_at_start()
{
    return m_blend_point_start;
}

gp_Pnt BlendCurve::compute_second_control_point_at_start()
{
    return m_blend_point_start.Translated(m_start.m_form_factor_1 / 3 * m_first_derivative_oriented_start_curve); 
}

gp_Pnt BlendCurve::compute_third_control_point_at_start()
{
    gp_Vec trans_vec = m_start.m_form_factor_1 / 6 * m_second_derivative_oriented_start_curve + (m_start.m_form_factor_2 / 6 + 2 / 3 * m_start.m_form_factor_1) * m_first_derivative_oriented_start_curve;
    return m_blend_point_start.Translated(trans_vec);
}

gp_Pnt BlendCurve::compute_first_control_point_at_end()
{
    return m_blend_point_end;
}

gp_Pnt BlendCurve::compute_second_control_point_at_end()
{
    return m_blend_point_end.Translated(m_end.m_form_factor_1 / 3 * (-m_first_derivative_oriented_end_curve));
}

gp_Pnt BlendCurve::compute_third_control_point_at_end()
{
    gp_Vec trans_vec = m_end.m_form_factor_1 / 6 * m_second_derivative_oriented_end_curve + (m_end.m_form_factor_2 / 6 + 2 / 3 * m_end.m_form_factor_1) * (-m_first_derivative_oriented_end_curve);
    return m_blend_point_end.Translated(trans_vec);
}

gp_Pnt BlendCurve::get_i_th_control_point(unsigned int i, GContinuity contin_start, GContinuity contin_end)
{
    int number_of_control_points = static_cast<int>(contin_start) + static_cast<int>(contin_end) + 2;

    if(i >= number_of_control_points)
    {
        throw geoml::Error("The index is too large", INDEX_ERROR);
    }

    if(contin_start == GContinuity::G0 && contin_end == GContinuity::G0)
    {
	    if(i==0) 
            return compute_first_control_point_at_start();
	    else if(i==1) 
            return compute_first_control_point_at_end();
	}
    else if(contin_start == GContinuity::G1 && contin_end == GContinuity::G0)
    {
        if (i==0) 
            return compute_first_control_point_at_start();
 	    else if(i==1) 
            return compute_second_control_point_at_start();
	    else if(i==2) 
            return compute_first_control_point_at_end();
    }
	else if(contin_start == GContinuity::G2 && contin_end == GContinuity::G0)
    {
        if (i==0) 
            return compute_first_control_point_at_start();
 	    else if(i==1) 
            return compute_second_control_point_at_start();
	    else if(i==2) 
            return compute_third_control_point_at_start();
        else if(i==3) 
            return compute_first_control_point_at_end();
    }
    else if(contin_start == GContinuity::G0 && contin_end == GContinuity::G1)
    {
	    if (i==0) 
            return compute_first_control_point_at_start();
	    else if(i==1) 
            return compute_second_control_point_at_end();
        else if(i==2) 
            return compute_first_control_point_at_end();
	}
    else if(contin_start == GContinuity::G1 && contin_end == GContinuity::G1)
    {
        if (i==0) 
            return compute_first_control_point_at_start();
 	    else if(i==1) 
            return compute_second_control_point_at_start();
	    else if(i==2) 
            return compute_second_control_point_at_end();
        else if(i==3) 
            return compute_first_control_point_at_end();
    }
	else if(contin_start == GContinuity::G2 && contin_end == GContinuity::G1)
    {
        if (i==0) 
            return compute_first_control_point_at_start();
 	    else if(i==1) 
            return compute_second_control_point_at_start();
	    else if(i==2) 
            return compute_third_control_point_at_start();
        else if(i==3) 
            return compute_second_control_point_at_end();
	    else if(i==4) 
            return compute_first_control_point_at_end();
    }
    else if(contin_start == GContinuity::G0 && contin_end == GContinuity::G2)
    {
        if (i==0) 
            return compute_first_control_point_at_start();
 	    else if(i==1) 
            return compute_third_control_point_at_end();
	    else if(i==2) 
            return compute_second_control_point_at_end();
        else if(i==3) 
            return compute_first_control_point_at_end();
    }
    else if(contin_start == GContinuity::G1 && contin_end == GContinuity::G2)
    {
        if (i==0) 
            return compute_first_control_point_at_start();
        else if(i==1) 
            return compute_second_control_point_at_start();
 	    else if(i==2) 
            return compute_third_control_point_at_end();
	    else if(i==3) 
            return compute_second_control_point_at_end();
        else if(i==4) 
            return compute_first_control_point_at_end();
    }
    else if(contin_start == GContinuity::G2 && contin_end == GContinuity::G2)
    {
        if (i==0) 
            return compute_first_control_point_at_start();
        else if(i==1) 
            return compute_second_control_point_at_start();
        else if(i==2) 
            return compute_third_control_point_at_start();
 	    else if(i==3) 
            return compute_third_control_point_at_end();
	    else if(i==4) 
            return compute_second_control_point_at_end();
        else if(i==5) 
            return compute_first_control_point_at_end();
    }
}

std::vector<gp_Pnt> BlendCurve::control_points_blend_curve()
{
    std::vector<gp_Pnt> cp_vec;
    int number_of_control_points = static_cast<int>(m_start.m_continuity) + static_cast<int>(m_end.m_continuity) + 2;

    for (int i = 0; i < number_of_control_points; ++i)
    {
        cp_vec.push_back(get_i_th_control_point(i, m_start.m_continuity, m_end.m_continuity));
    }

    return cp_vec;
}

TopoDS_Edge BlendCurve::blend_curve()
{
    std::vector cp_vec = control_points_blend_curve();
    TColgp_Array1OfPnt cp_tcol = StdVector_to_TCol(cp_vec);

    Handle(Geom_BezierCurve) bezier_curve = new Geom_BezierCurve(cp_tcol);

    return CurveToEdge(bezier_curve);
}

} // namespace geoml