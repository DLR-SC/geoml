#include "curves/BlendCurve.h"

#include "geoml/error.h"

#include <BRep_Tool.hxx>
#include <Precision.hxx>

#include <cmath>

namespace geoml{

BlendCurveConnection::BlendCurveConnection(TopoDS_Edge const& edge, gp_Pnt const& near_connection_point, GContinuity continuity, bool outward_direction, Standard_Real beta, Standard_Real gamma)
: m_near_connection_point(near_connection_point)
, m_continuity(continuity)
, m_beta(beta)
, m_gamma(gamma)
, m_outward_direction(outward_direction)
, m_curve(BRep_Tool::Curve(edge, m_curve_first_param, m_curve_last_param))
{
    compute_blend_parameter();
} 

void BlendCurveConnection::compute_blend_parameter()
{
    Standard_Real distance_first_point_on_curve_to_near_connection_point = m_curve->Value(m_curve_first_param).SquareDistance(m_near_connection_point);  

    Standard_Real distance_last_point_on_curve_to_near_connection_point = m_curve->Value(m_curve_last_param).SquareDistance(m_near_connection_point);

    if (std::abs(distance_first_point_on_curve_to_near_connection_point - distance_last_point_on_curve_to_near_connection_point) < Precision::SquareConfusion())
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
    m_degree = static_cast<int>(m_start.m_continuity) + static_cast<int>(m_end.m_continuity) + 1;
}

void BlendCurve::compute_blend_points_and_derivatives_of_start_and_end_curve()
{
    m_start.m_curve->D2(m_start.m_curve_blend_param, m_blend_point_start, m_first_derivative_start_curve, m_second_derivative_start_curve);
    m_end.m_curve->D2(m_end.m_curve_blend_param, m_blend_point_end, m_first_derivative_end_curve, m_second_derivative_end_curve);
}

gp_Pnt BlendCurve::control_point_2(gp_Pnt first_point, Standard_Real beta, gp_Vec first_derivative)
{
    return first_point.Translated(beta * first_derivative);
}

gp_Pnt BlendCurve::control_point_3(gp_Pnt first_point, Standard_Real beta, Standard_Real gamma, gp_Vec first_derivative, gp_Vec second_derivative)
{
    gp_Vec displacement_vector = (beta * beta * m_degree /(m_degree - 1)) * second_derivative;

    Standard_Real scalar_product_displ_vec_first_derivative = displacement_vector.Dot(first_derivative); 
    Standard_Real square_magnitude_first_derivative = first_derivative.Dot(first_derivative);
   
    gp_Vec h1 = displacement_vector - scalar_product_displ_vec_first_derivative*first_derivative / square_magnitude_first_derivative;
    gp_Vec h2 = 2.*(gamma*beta*first_derivative);

    return first_point.Translated(h1 + h2);
}

gp_Pnt BlendCurve::compute_control_point_2_at_start()
{
    int sign = m_start.m_outward_direction? 1 : -1;

    if (std::abs (m_start.m_curve_blend_param - m_start.m_curve_last_param) < Precision::PConfusion())
    {
        return control_point_2 (m_blend_point_start, m_start.m_beta, sign * m_first_derivative_start_curve);
    }
    else if (std::abs(m_start.m_curve_blend_param - m_start.m_curve_first_param) < Precision::PConfusion())
    {
        return control_point_2(m_blend_point_start, m_start.m_beta, -sign * m_first_derivative_start_curve); 
    }
    else
    {
        throw geoml::Error("Blending parameter must match either the first or the last parameter of the start curve", MATH_ERROR);
    }
}

gp_Pnt BlendCurve::compute_control_point_3_at_start()
{
    if(m_start.m_outward_direction)
    {
        if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_last_param) < Precision::PConfusion())
        {
            return control_point_3(m_blend_point_start, m_start.m_beta, m_start.m_gamma, m_first_derivative_start_curve, m_second_derivative_start_curve);
        }
        else if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_first_param) < Precision::PConfusion())
        {    
            return control_point_3(m_blend_point_start, m_start.m_beta, -m_start.m_gamma, m_first_derivative_start_curve, m_second_derivative_start_curve);
        }
        else
        {
            throw geoml::Error("tolerance error", MATH_ERROR);
        }

    } else {

        if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_last_param) < Precision::PConfusion())
        {
            return control_point_3(m_blend_point_start, m_start.m_beta, -m_start.m_gamma, m_first_derivative_start_curve, m_second_derivative_start_curve);
        }
        else if(std::abs(m_start.m_curve_blend_param - m_start.m_curve_first_param) < Precision::PConfusion())
        {    
            return control_point_3(m_blend_point_start, m_start.m_beta, m_start.m_gamma, m_first_derivative_start_curve, m_second_derivative_start_curve);
        }
        else
        {
            throw geoml::Error("tolerance error", MATH_ERROR);
        }
        
    }   
}

gp_Pnt BlendCurve::compute_control_point_2_at_end()
{
    if(m_end.m_outward_direction)
    {
        if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_first_param) < Precision::PConfusion())
        {
            return control_point_2(m_blend_point_end, m_end.m_beta, -m_first_derivative_end_curve); 
        }
        else if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_last_param) < Precision::PConfusion())
        {    
            return control_point_2(m_blend_point_end, m_end.m_beta, m_first_derivative_end_curve); 
        }
        else
        {
            throw geoml::Error("tolerance error", MATH_ERROR);
        }

    } else {

        if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_first_param) < Precision::PConfusion())
        {
            return control_point_2(m_blend_point_end, m_end.m_beta, m_first_derivative_end_curve);
        }
        else if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_last_param) < Precision::PConfusion())
        {    
            return control_point_2(m_blend_point_end, m_end.m_beta, -m_first_derivative_end_curve);
        }
        else
        {
            throw geoml::Error("tolerance error", MATH_ERROR);
        }
        
    }
}

gp_Pnt BlendCurve::compute_control_point_3_at_end()
{
    if(m_end.m_outward_direction)
    {
        if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_first_param) < Precision::PConfusion())
        {
            return control_point_3(m_blend_point_end, m_end.m_beta, -m_end.m_gamma, m_first_derivative_end_curve, m_second_derivative_end_curve);
        }
        else if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_last_param) < Precision::PConfusion())
        {    
            return control_point_3(m_blend_point_end, m_end.m_beta, m_end.m_gamma, m_first_derivative_end_curve, m_second_derivative_end_curve);
        }
        else
        {
            throw geoml::Error("tolerance error", MATH_ERROR);
        }

    } else {

        if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_first_param) < Precision::PConfusion())
        {
            return control_point_3(m_blend_point_end, m_end.m_beta, m_end.m_gamma, m_first_derivative_end_curve, m_second_derivative_end_curve);
        }
        else if(std::abs(m_end.m_curve_blend_param - m_end.m_curve_last_param) < Precision::PConfusion())
        {    
            return control_point_3(m_blend_point_end, m_end.m_beta, -m_end.m_gamma, m_first_derivative_end_curve, m_second_derivative_end_curve);
        }
        else
        {
            throw geoml::Error("tolerance error", MATH_ERROR);
        }
        
    }
}

gp_Pnt BlendCurve::get_i_th_control_point(unsigned int i, GContinuity contin_start, GContinuity contin_end)
{
    int number_of_control_points = m_degree + 1;

    if(i >= number_of_control_points)
    {
        throw geoml::Error("The index is too large", INDEX_ERROR);
    }

    if(contin_start == GContinuity::G0 && contin_end == GContinuity::G0)
    {
	    if(i==0) 
            return m_blend_point_start;
	    else  
            return m_blend_point_end;
            
	}
    else if(contin_start == GContinuity::G1 && contin_end == GContinuity::G0)
    {
        if (i==0) 
            return m_blend_point_start;
 	    else if(i==1) 
            return compute_control_point_2_at_start();
	    else 
            return m_blend_point_end;
    }
	else if(contin_start == GContinuity::G2 && contin_end == GContinuity::G0)
    {
        if (i==0) 
            return m_blend_point_start;
 	    else if(i==1) 
            return compute_control_point_2_at_start();
	    else if(i==2) 
            return compute_control_point_3_at_start();
        else 
            return m_blend_point_end;
    }
    else if(contin_start == GContinuity::G0 && contin_end == GContinuity::G1)
    {
	    if (i==0) 
            return m_blend_point_start;
	    else if(i==1) 
            return compute_control_point_2_at_end();
        else 
            return m_blend_point_end;
	}
    else if(contin_start == GContinuity::G1 && contin_end == GContinuity::G1)
    {
        if (i==0) 
            return m_blend_point_start;
 	    else if(i==1) 
            return compute_control_point_2_at_start();
	    else if(i==2) 
            return compute_control_point_2_at_end();
        else 
            return m_blend_point_end;
    }
	else if(contin_start == GContinuity::G2 && contin_end == GContinuity::G1)
    {
        if (i==0) 
            return m_blend_point_start;
 	    else if(i==1) 
            return compute_control_point_2_at_start();
	    else if(i==2) 
            return compute_control_point_3_at_start();
        else if(i==3) 
            return compute_control_point_2_at_end();
	    else
            return m_blend_point_end;
    }
    else if(contin_start == GContinuity::G0 && contin_end == GContinuity::G2)
    {
        if (i==0) 
            return m_blend_point_start;
 	    else if(i==1) 
            return compute_control_point_3_at_end();
	    else if(i==2) 
            return compute_control_point_2_at_end();
        else
            return m_blend_point_end;
    }
    else if(contin_start == GContinuity::G1 && contin_end == GContinuity::G2)
    {
        if (i==0) 
            return m_blend_point_start;
        else if(i==1) 
            return compute_control_point_2_at_start();
 	    else if(i==2) 
            return compute_control_point_3_at_end();
	    else if(i==3) 
            return compute_control_point_2_at_end();
        else 
            return m_blend_point_end;
    }
    else if(contin_start == GContinuity::G2 && contin_end == GContinuity::G2)
    {
        if (i==0) 
            return m_blend_point_start;
        else if(i==1) 
            return compute_control_point_2_at_start();
        else if(i==2) 
            return compute_control_point_3_at_start();
 	    else if(i==3) 
            return compute_control_point_3_at_end();
	    else if(i==4) 
            return compute_control_point_2_at_end();
        else
            return m_blend_point_end;
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