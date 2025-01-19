#include "curves/BlendCurve.h"

#include "geoml/error.h"

#include <cmath>

#include <BRep_Tool.hxx>


namespace geoml{

BlendCurveConnection::BlendCurveConnection(TopoDS_Edge const& edge, gp_Pnt const& near_connection_point, GContinuity continuity, bool outward_direction, Standard_Real beta, Standard_Real gamma)
: m_near_connection_point(near_connection_point)
, m_continuity(continuity)
, m_beta(beta)
, m_gamma(gamma)
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
    compute_blend_points_and_derivatives_of_start_and_end_curve();
}

void BlendCurve::compute_blend_points_and_derivatives_of_start_and_end_curve()
{
    m_start.m_curve->D2(m_start.m_curve_blend_param, m_blend_point_start, m_first_derivative_start_curve, m_second_derivative_start_curve);
    m_end.m_curve->D2(m_end.m_curve_blend_param, m_blend_point_end, m_first_derivative_end_curve, m_second_derivative_end_curve);
}

gp_Pnt BlendCurve::formula_for_second_control_point_in_parameter_direction(gp_Pnt first_point, Standard_Real beta, gp_Vec first_derivative)
{
    return first_point.Translated(beta / 3 * first_derivative);
}

gp_Pnt BlendCurve::formula_for_second_control_point_against_parameter_direction(gp_Pnt first_point, Standard_Real beta, gp_Vec first_derivative)
{
    return first_point.Translated(beta / 3 * (-first_derivative));
}

gp_Pnt BlendCurve::formula_for_third_control_point_in_parameter_direction(gp_Pnt first_point, Standard_Real beta, Standard_Real gamma, gp_Vec first_derivative, gp_Vec second_derivative)
{
    return first_point.Translated(pow(beta, 2)/6 * second_derivative 
        - pow(beta, 2) / (6 * pow(first_derivative.Magnitude(), 2)) * second_derivative.Dot(first_derivative) * first_derivative 
        + 2 / 3 * gamma * beta * first_derivative);
}

gp_Pnt BlendCurve::formula_for_third_control_point_against_parameter_direction(gp_Pnt first_point, Standard_Real beta, Standard_Real gamma, gp_Vec first_derivative, gp_Vec second_derivative)
{
    return first_point.Translated(pow(beta, 2)/6 * second_derivative 
        - pow(beta, 2) / (6 * pow(first_derivative.Magnitude(), 2)) * second_derivative.Dot(first_derivative) * first_derivative 
        - 2 / 3 * gamma * beta * first_derivative);
}

gp_Pnt BlendCurve::compute_first_control_point_at_start()
{
    return m_blend_point_start;
}

gp_Pnt BlendCurve::compute_second_control_point_at_start()
{
    if(m_start.m_outward_direction)
    {
        if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_last_param) < 1e-5)
        {        
            return formula_for_second_control_point_in_parameter_direction(m_blend_point_start, m_start.m_beta, m_first_derivative_start_curve); 
        }
        else if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_first_param) < 1e-5)
        {    
            return formula_for_second_control_point_against_parameter_direction(m_blend_point_start, m_start.m_beta, m_first_derivative_start_curve);
        }

    } else {

        if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_last_param) < 1e-5)
        {
            return formula_for_second_control_point_against_parameter_direction(m_blend_point_start, m_start.m_beta, m_first_derivative_start_curve);
        }
        else if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_first_param) < 1e-5)
        {    
            return formula_for_second_control_point_in_parameter_direction(m_blend_point_start, m_start.m_beta, m_first_derivative_start_curve); 
        }
        
    }
}

gp_Pnt BlendCurve::compute_third_control_point_at_start()
{
    if(m_start.m_outward_direction)
    {
        if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_last_param) < 1e-5)
        {
            return formula_for_third_control_point_in_parameter_direction(m_blend_point_start, m_start.m_beta, m_start.m_gamma, m_first_derivative_start_curve, m_second_derivative_start_curve);
        }
        else if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_first_param) < 1e-5)
        {    
            return formula_for_third_control_point_against_parameter_direction(m_blend_point_start, m_start.m_beta, m_start.m_gamma, m_first_derivative_start_curve, m_second_derivative_start_curve);
        }

    } else {

        if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_last_param) < 1e-5)
        {
            return formula_for_third_control_point_against_parameter_direction(m_blend_point_start, m_start.m_beta, m_start.m_gamma, m_first_derivative_start_curve, m_second_derivative_start_curve);
        }
        else if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_first_param) < 1e-5)
        {    
            return formula_for_third_control_point_in_parameter_direction(m_blend_point_start, m_start.m_beta, m_start.m_gamma, m_first_derivative_start_curve, m_second_derivative_start_curve);
        }
        
    }   
}

gp_Pnt BlendCurve::compute_first_control_point_at_end()
{
    return m_blend_point_end;
}

gp_Pnt BlendCurve::compute_second_control_point_at_end()
{
    if(m_end.m_outward_direction)
    {
        if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_first_param) < 1e-5)
        {
            return formula_for_second_control_point_against_parameter_direction(m_blend_point_end, m_end.m_beta, m_first_derivative_end_curve); 
        }
        else if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_last_param) < 1e-5)
        {    
            return formula_for_second_control_point_in_parameter_direction(m_blend_point_end, m_end.m_beta, m_first_derivative_end_curve); 
        }

    } else {

        if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_first_param) < 1e-5)
        {
            return formula_for_second_control_point_in_parameter_direction(m_blend_point_end, m_end.m_beta, m_first_derivative_end_curve);
        }
        else if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_last_param) < 1e-5)
        {    
            return formula_for_second_control_point_against_parameter_direction(m_blend_point_end, m_end.m_beta, m_first_derivative_end_curve);
        }
        
    }
}

gp_Pnt BlendCurve::compute_third_control_point_at_end()
{
    if(m_end.m_outward_direction)
    {
        if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_first_param) < 1e-5)
        {
            return formula_for_third_control_point_against_parameter_direction(m_blend_point_end, m_end.m_beta, m_end.m_gamma, m_first_derivative_end_curve, m_second_derivative_end_curve);
        }
        else if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_last_param) < 1e-5)
        {    
            return formula_for_third_control_point_in_parameter_direction(m_blend_point_end, m_end.m_beta, m_end.m_gamma, m_first_derivative_end_curve, m_second_derivative_end_curve);
        }

    } else {

        if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_first_param) < 1e-5)
        {
            return formula_for_third_control_point_in_parameter_direction(m_blend_point_end, m_end.m_beta, m_end.m_gamma, m_first_derivative_end_curve, m_second_derivative_end_curve);
        }
        else if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_last_param) < 1e-5)
        {    
            return formula_for_third_control_point_against_parameter_direction(m_blend_point_end, m_end.m_beta, m_end.m_gamma, m_first_derivative_end_curve, m_second_derivative_end_curve);
        }
        
    }
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
    else
    {
        throw geoml::Error("Wrong arguments for GContinuity.", GENERIC_ERROR);
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

TopoDS_Edge blend_curve(BlendCurveConnection const& start, BlendCurveConnection const& end)
{
    BlendCurve my_blend_curve (start, end);
    return my_blend_curve.blend_curve();
}

} // namespace geoml