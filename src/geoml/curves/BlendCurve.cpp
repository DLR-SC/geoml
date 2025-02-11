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
    compute_blend_point_and_derivatives();
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

void BlendCurveConnection::compute_blend_point_and_derivatives()
{
    m_curve->D2(m_curve_blend_param, m_blend_point, m_first_derivative_curve, m_second_derivative_curve);
}

BlendCurve::BlendCurve(BlendCurveConnection const& start, BlendCurveConnection const& end)
    : m_start(start)
    , m_end(end)
    , m_degree(static_cast<int>(m_start.m_continuity) + static_cast<int>(m_end.m_continuity) + 1)
{ 
    m_start.m_start = true;
    m_end.m_start = false;
}

gp_Pnt BlendCurve::control_point_2(BlendCurveConnection &side)
{
    int sign = side.m_outward_direction? 1 : -1;

    auto control_point_2_formula = [](gp_Pnt& first_point, Standard_Real beta, const gp_Vec& first_derivative)
                            {
                                return first_point.Translated(beta * first_derivative);
                            };

    if (std::abs (side.m_curve_blend_param - side.m_curve_last_param) < Precision::PConfusion())
    {
        return control_point_2_formula(side.m_blend_point, side.m_beta, sign * side.m_first_derivative_curve);
    }
    else if (std::abs(side.m_curve_blend_param - side.m_curve_first_param) < Precision::PConfusion())
    {
        return control_point_2_formula(side.m_blend_point, side.m_beta, -sign * side.m_first_derivative_curve); 
    }
    else
    {
        throw geoml::Error("Blending parameter must match either the first or the last parameter of the start curve", MATH_ERROR);
    }
}

gp_Pnt BlendCurve::control_point_3(BlendCurveConnection &side)
{
    int sign = side.m_outward_direction? 1 : -1;

    auto control_point_3_formula = [](gp_Pnt first_point, Standard_Real beta, Standard_Real gamma, gp_Vec first_derivative, gp_Vec second_derivative, int degree)
                        {
                            gp_Vec displacement_vector = (beta * beta * degree /(degree - 1)) * second_derivative;

                            Standard_Real scalar_product_displ_vec_first_derivative = displacement_vector.Dot(first_derivative); 
                            Standard_Real square_magnitude_first_derivative = first_derivative.Dot(first_derivative);
                        
                            gp_Vec h1 = displacement_vector - scalar_product_displ_vec_first_derivative*first_derivative / square_magnitude_first_derivative;
                            gp_Vec h2 = 2.*(gamma*beta*first_derivative);

                            return first_point.Translated(h1 + h2);
                        };

    if (std::abs (side.m_curve_blend_param - side.m_curve_last_param) < Precision::PConfusion())
    {
        return control_point_3_formula(side.m_blend_point, side.m_beta, sign * side.m_gamma, side.m_first_derivative_curve, side.m_second_derivative_curve, m_degree);
    }
    else if (std::abs(side.m_curve_blend_param - side.m_curve_first_param) < Precision::PConfusion())
    {
        return control_point_3_formula(side.m_blend_point, side.m_beta, -sign * side.m_gamma, side.m_first_derivative_curve, side.m_second_derivative_curve, m_degree); 
    }
    else
    {
        throw geoml::Error("Blending parameter must match either the first or the last parameter of the start curve", MATH_ERROR);
    } 
}

gp_Pnt BlendCurve::compute_control_point(int index, BlendCurveConnection &side)
{
    switch (index) 
    {
    case 0:
        if(side.m_start == true)
        {
            return m_start.m_blend_point;
        }
        else
        {
            return m_end.m_blend_point;
        }
    case 1:
        if(side.m_start == true)
        {
            return control_point_2(m_start);
        }
        else
        {
            return control_point_2(m_end);
        }
    case 2:
        if(side.m_start == true)
        {
            return control_point_3(m_start);
        }
        else
        {
            return control_point_3(m_end);
        }
    default:
        throw geoml::Error("The index is not equal to 0,1 or 2.", INDEX_ERROR);
    }
}

std::vector<gp_Pnt> BlendCurve::control_points_blend_curve()
{
    std::vector<gp_Pnt> cp_vec;
    int number_of_control_points = static_cast<int>(m_start.m_continuity) + static_cast<int>(m_end.m_continuity) + 2;

    cp_vec.reserve(number_of_control_points);

    for (int i=0; i < static_cast<int>(m_start.m_continuity)+1; ++i) {
        cp_vec.push_back(compute_control_point(i, m_start));
    }

    for (int i=static_cast<int>(m_end.m_continuity); i >= 0; --i) {
        cp_vec.push_back(compute_control_point(i, m_end));
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