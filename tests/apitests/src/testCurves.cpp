#include <geoml/curves/curves.h>
#include <geoml/curves/BlendCurve.h>
#include "geometry/BSplineAlgorithms.h"
#include "geoml/data_structures/conversions.h"
#include "geoml/Continuity.h"
#include "geoml/geom_topo_conversions/geom_topo_conversions.h"

#include <gtest/gtest.h>

#include <gp_Pnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include "BRepBuilderAPI_MakeEdge.hxx"
#include <TopoDS_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>

TEST(Test_nurbs_curve, closed_unclamped_nurbs_curve)
{    

// define a closed B-spline curve of 
// degree 2 by overlapping two control points (same number of overlapping control points as the degree) and 
// a uniform choice of knots      

// define control points
gp_Pnt p_1(0.0, 0.0, 0.0);
gp_Pnt p_2(0.0, 1.0, 2.0);
gp_Pnt p_3(0.0, 2.0, -1.0);
gp_Pnt p_4(0.0, 0.0, 0.0);
gp_Pnt p_5(0.0, 1.0, 2.0);

std::vector<gp_Pnt> control_points {p_1,p_2,p_3,p_4,p_5};

// degree:
Standard_Integer degree = 2;

// weights (non-rational):
std::vector<Standard_Real> weights (5,1.0);

// knots: note: as the number of knots (m+1), the number of control points (p+1) and the degree p
// are related by m = n + p +1, the number of knots has to be 8 in this case.

std::vector<Standard_Real> knots {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};

// multiplicities: 
std::vector<int> mults(8,1);

Handle(Geom_BSplineCurve) curve =
    geoml::nurbs_curve(
        control_points, 
        weights, 
        knots, 
        mults, 
        degree);

// expect the start and end point of the curve to coincide
EXPECT_NEAR(
    curve->StartPoint().X(), 
    curve->EndPoint().X(), 
    1e-5);

EXPECT_NEAR(
    curve->StartPoint().Y(), 
    curve->EndPoint().Y(), 
    1e-5);

EXPECT_NEAR(
    curve->StartPoint().Z(), 
    curve->EndPoint().Z(), 
    1e-5);

// expect the curve to be non-rational, as all weights are set to 1
EXPECT_EQ(curve->IsRational(), false); 

}

TEST(Test_nurbs_curve, open_clamped_nurbs_curve)
{

// define an open and clamped NURBS curve of degree 2

// control points
gp_Pnt p1(0.0, 0.0, 0.0);
gp_Pnt p2(0.0, 1.0, 2.0);
gp_Pnt p3(0.0, 2.0, -1.0);
gp_Pnt p4(1.0, 0.0, 3.0);
gp_Pnt p5(0.0, 4.0, 0.0);

std::vector<gp_Pnt> control_points {p1, p2, p3, p4, p5};

// degree:
Standard_Integer degree = 2;

// weights:
std::vector<Standard_Real> weights(5,1.0);

// knots: note: as the number of knots (m+1), the number of control points (n+1) and the degree p
// are related by m = n + p +1, the number of knots has to be 8 in this case. To achive this, in the following the
// first and the last knot have to appear trice (have multiplicity of 3)

std::vector<Standard_Real> knots{0.0, 1.0, 2.0, 3.0};

// multiplicities: 
std::vector<int> mults {3, 1, 1, 3};

Handle(Geom_BSplineCurve) curve =
    geoml::nurbs_curve(
        control_points, 
        weights, 
        knots, 
        mults, 
        degree);

EXPECT_NEAR(
    curve->StartPoint().X(), 
    p1.X(), 
    1e-5);

EXPECT_NEAR(
    curve->StartPoint().Y(), 
    p1.Y(), 
    1e-5);

EXPECT_NEAR(
    curve->StartPoint().Z(), 
    p1.Z(), 
    1e-5);

EXPECT_NEAR(
    curve->EndPoint().X(), 
    p5.X(), 
    1e-5);

EXPECT_NEAR(
    curve->EndPoint().Y(), 
    p5.Y(), 
    1e-5);

EXPECT_NEAR(
    curve->EndPoint().Z(), 
    p5.Z(), 
    1e-5);

}

TEST(Test_nurbs_curve, circle_rational_bspline_three_arcs)
{

// define a circle via a rational B-spline curve
// of degree 2:

// control points
gp_Pnt p_1(0.0, 0.0, 2.0);
gp_Pnt p_2(0.0, 3.464, 2.0);
gp_Pnt p_3(0.0, 1.732, -1.0);
gp_Pnt p_4(0.0, 0.0, -4.0);
gp_Pnt p_5(0.0, -1.732, -1.0);
gp_Pnt p_6(0.0, -3.464, 2.0);
gp_Pnt p_7(0.0, 0.0, 2.0);

std::vector<gp_Pnt> control_points {p_1, p_2, p_3, p_4, p_5, p_6, p_7};
  
// degree:
Standard_Integer degree = 2;

// weights:
std::vector<Standard_Real> weights {1.0, 0.5, 1.0, 0.5, 1.0, 0.5, 1.0};

// knots: (we need 10 knots (including their multiplicities), which follows from "#knots = #control_points + degree + 1"
std::vector<Standard_Real> knots {0.0, 1.0, 2.0, 3.0};

// multiplicities: 
std::vector<int> mults {3, 2, 2, 3};

Handle(Geom_BSplineCurve) curve =
    geoml::nurbs_curve(
        control_points, 
        weights, 
        knots, 
        mults, 
        degree);

// get the length of the curve
Standard_Real umin = curve->FirstParameter();
Standard_Real umax = curve->LastParameter();
GeomAdaptor_Curve adaptorCurve(curve, umin, umax);
Standard_Real length = GCPnts_AbscissaPoint::Length(adaptorCurve, umin, umax);

// Expect the circumference of the circle.
EXPECT_NEAR(length, 12.566, 1e-3);
EXPECT_EQ(curve->NbPoles(), 7);
EXPECT_EQ(curve->NbKnots(), 4); // note, the number of knots OCC gives is not counting the multiplicities of the knots!
EXPECT_FALSE(curve->IsPeriodic());

EXPECT_EQ(curve->Multiplicity(1), 3);
EXPECT_EQ(curve->Multiplicity(2), 2);
EXPECT_EQ(curve->Multiplicity(3), 2);
EXPECT_EQ(curve->Multiplicity(4), 3);

curve->SetPeriodic();

EXPECT_TRUE(curve->IsPeriodic());
EXPECT_EQ(curve->NbPoles(), 6); // the number of control point differs from the curve before envoking SetPeriodic() 
EXPECT_EQ(curve->NbKnots(), 4);

EXPECT_EQ(curve->Multiplicity(1), 2); // the multiplicities differ, too. 
EXPECT_EQ(curve->Multiplicity(2), 2);
EXPECT_EQ(curve->Multiplicity(3), 2);
EXPECT_EQ(curve->Multiplicity(4), 2);

}

TEST(Test_interpolate_points_to_b_spline_curve, simple_interpolation_of_points)
{
    // points to interpolate
    gp_Pnt pt_1 (0.0, 0.0, 0.0);
    gp_Pnt pt_2 (1.0, 0.0, 0.0);
    gp_Pnt pt_3 (2.0, 0.0, 1.0);

    std::vector <gp_Pnt> input_points {pt_1, pt_2, pt_3};

    // create B-spline curve interpolating the points
    Handle(Geom_BSplineCurve) curve =
    geoml::interpolate_points_to_b_spline_curve(input_points);

    // check if start and end points are interpolated
    EXPECT_NEAR(curve->StartPoint().X(), pt_1.X(), 1e-5);
    EXPECT_NEAR(curve->StartPoint().Y(), pt_1.Y(), 1e-5);
    EXPECT_NEAR(curve->StartPoint().Z(), pt_1.Z(), 1e-5);

    EXPECT_NEAR(curve->EndPoint().X(), pt_3.X(), 1e-5);
    EXPECT_NEAR(curve->EndPoint().Y(), pt_3.Y(), 1e-5);
    EXPECT_NEAR(curve->EndPoint().Z(), pt_3.Z(), 1e-5);

    // calculate the curve parameters of the interpolated points
    Handle(TColgp_HArray1OfPnt) points_col = new TColgp_HArray1OfPnt(1, input_points.size());
    for(int i = 0; i < input_points.size(); ++i)
    {
        points_col->SetValue(i + 1, input_points.at(i));
    }

    std::vector<double> params = geoml::BSplineAlgorithms::computeParamsBSplineCurve(points_col);
    
    // check if the first and last points match with the right curve parameters
    EXPECT_NEAR(params[0], 0, 1e-5);
    EXPECT_NEAR(params[2], 1, 1e-5);

    // check if the second point is interpolated
    gp_Pnt sec_point = curve -> Value(params[1]);
    double dist = pt_2.Distance(sec_point);

    EXPECT_NEAR(dist, 0, 1e-5);

}

TEST(Test_interpolate_points_to_b_spline_curve, interpolation_with_lower_degree)
{
    gp_Pnt pt_1 (0.0, 0.0, 0.0);
    gp_Pnt pt_2 (1.0, 0.0, 0.0);
    gp_Pnt pt_3 (2.0, 0.0, 1.0);

    std::vector <gp_Pnt> input_points {pt_1, pt_2, pt_3};

    Handle(Geom_BSplineCurve) curve_1 =
    geoml::interpolate_points_to_b_spline_curve(input_points, 1);

    EXPECT_EQ(curve_1->Degree(), 1);

    Handle(Geom_BSplineCurve) curve_2 =
    geoml::interpolate_points_to_b_spline_curve(input_points, 5);
    EXPECT_EQ(curve_2->Degree(), 2);

}

TEST(Test_interpolate_points_to_b_spline_curve, interpolation_closed_curve)
{
    gp_Pnt pt_1 (0.0, 0.0, 0.0);
    gp_Pnt pt_2 (1.0, 0.0, 0.0);
    gp_Pnt pt_3 (2.0, 0.0, 1.0);
    gp_Pnt pt_4 (0.0, 0.0, 0.0);

    std::vector <gp_Pnt> input_points {pt_1, pt_2, pt_3, pt_4};

    Handle(Geom_BSplineCurve) curve_1 =
    geoml::interpolate_points_to_b_spline_curve(input_points, 3, false);

    EXPECT_EQ(curve_1->NbPoles(), 4);

    Handle(Geom_BSplineCurve) curve_2 =
    geoml::interpolate_points_to_b_spline_curve(input_points, 3, true);

    EXPECT_EQ(curve_2->NbPoles(), 6);

}

TEST(Test_interpolate_points_to_b_spline_curve, interpolation_custom_parameters)
{
    gp_Pnt pt_1 (0.0, 0.0, 0.0);
    gp_Pnt pt_2 (1.0, 0.0, 0.0);
    gp_Pnt pt_3 (2.0, 0.0, 1.0);

    std::vector <gp_Pnt> input_points {pt_1, pt_2, pt_3};
    std::vector <Standard_Real> params {0., 0.90, 1.};

    Handle(Geom_BSplineCurve) curve =
    geoml::interpolate_points_to_b_spline_curve(input_points, 3, false, params);

    gp_Pnt test_point = curve->Value(0.9);

    EXPECT_NEAR(test_point.X(), 1.0, 1e-5);
    EXPECT_NEAR(test_point.Y(), 0.0, 1e-5);
    EXPECT_NEAR(test_point.Z(), 0.0, 1e-5);

}

gp_Vec displacement_of_third_control_point(int degree, Standard_Real beta, Standard_Real gamma, gp_Vec first_derivative, gp_Vec second_derivative)
{
    gp_Vec vec = (beta * beta * degree /(degree - 1)) * second_derivative;
    Standard_Real scalar_product_vec_first_derivative = vec.Dot(first_derivative); 
    Standard_Real square_magnitude_first_derivative = first_derivative.Dot(first_derivative);
   
    return vec - scalar_product_vec_first_derivative*first_derivative / square_magnitude_first_derivative;
}

class CurvesToBlend : public ::testing::Test
{
protected:

    void SetUp() override
    {
        cp_start_1 = gp_Pnt(0.,0.,0.);
        cp_start_2 = gp_Pnt(2.,0.,0.);
        cp_start_3 = gp_Pnt(2.,2.,0.);
    
        cp_end_1 = gp_Pnt(4.,4.,0.);
        cp_end_2 = gp_Pnt(6.,4.,0.);
        cp_end_3 = gp_Pnt(6.,6.,0.);

        std::vector<gp_Pnt> control_points_start_curve{cp_start_1, cp_start_2, cp_start_3};
        std::vector<gp_Pnt> control_points_end_curve{cp_end_1, cp_end_2, cp_end_3};

        start_curve = new Geom_BezierCurve(geoml::StdVector_to_TCol(control_points_start_curve));
        end_curve = new Geom_BezierCurve(geoml::StdVector_to_TCol(control_points_end_curve));

        // compute derivatives of start_curve and end_curve
        first_parameter_start_curve = start_curve->FirstParameter();
        last_parameter_start_curve = start_curve->LastParameter();

        first_parameter_end_curve = end_curve->FirstParameter();
        last_parameter_end_curve = end_curve->LastParameter();

        start_curve->D2(first_parameter_start_curve, first_point_start_curve, first_derivative_start_curve_at_first_parameter, second_derivative_start_curve_at_first_parameter);
        start_curve->D2(last_parameter_start_curve, last_point_start_curve, first_derivative_start_curve_at_last_parameter, second_derivative_start_curve_at_last_parameter);

        end_curve->D2(first_parameter_end_curve, first_point_end_curve, first_derivative_end_curve_at_first_parameter, second_derivative_end_curve_at_first_parameter);
        end_curve->D2(last_parameter_end_curve, last_point_end_curve, first_derivative_end_curve_at_last_parameter, second_derivative_end_curve_at_last_parameter);
    }

    void TearDown() override
    {
    }

    gp_Pnt cp_start_1;
    gp_Pnt cp_start_2;
    gp_Pnt cp_start_3;

    gp_Pnt cp_end_1;
    gp_Pnt cp_end_2;
    gp_Pnt cp_end_3;

    Handle(Geom_Curve) start_curve;
    Handle(Geom_Curve) end_curve;

    Standard_Real first_parameter_start_curve;
    Standard_Real last_parameter_start_curve;

    Standard_Real first_parameter_end_curve;
    Standard_Real last_parameter_end_curve;

    gp_Pnt first_point_start_curve;
    gp_Pnt last_point_start_curve;

    gp_Vec first_derivative_start_curve_at_first_parameter;
    gp_Vec first_derivative_start_curve_at_last_parameter;

    gp_Vec second_derivative_start_curve_at_first_parameter;
    gp_Vec second_derivative_start_curve_at_last_parameter;

    gp_Pnt first_point_end_curve;
    gp_Pnt last_point_end_curve;

    gp_Vec first_derivative_end_curve_at_first_parameter;
    gp_Vec first_derivative_end_curve_at_last_parameter;

    gp_Vec second_derivative_end_curve_at_first_parameter;
    gp_Vec second_derivative_end_curve_at_last_parameter;
};

TEST_F(CurvesToBlend, blend_curve_test_1)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G0);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G0);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve  = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve  = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 2);
    
    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_2)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G1);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G0);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve  = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve  = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 3);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), last_point_start_curve.X() + first_derivative_start_curve_at_last_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), last_point_start_curve.Y() + first_derivative_start_curve_at_last_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), last_point_start_curve.Z() + first_derivative_start_curve_at_last_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_3)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G0);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G1);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve  = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve  = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 3);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), first_point_end_curve.X() - first_derivative_end_curve_at_first_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), first_point_end_curve.Y() - first_derivative_end_curve_at_first_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), first_point_end_curve.Z() - first_derivative_end_curve_at_first_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_4)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G2);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G0);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 4);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), last_point_start_curve.X() + first_derivative_start_curve_at_last_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), last_point_start_curve.Y() + first_derivative_start_curve_at_last_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), last_point_start_curve.Z() + first_derivative_start_curve_at_last_parameter.Z(), 1e-5);

    gp_Vec d_vec_curve_start = displacement_of_third_control_point(bezier_curve->Degree(), 1., 1., first_derivative_start_curve_at_last_parameter, second_derivative_start_curve_at_last_parameter);

    EXPECT_NEAR(bezier_curve->Pole(3).X(), last_point_start_curve.X() + 2 * first_derivative_start_curve_at_last_parameter.X() + d_vec_curve_start.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Y(), last_point_start_curve.Y() + 2 * first_derivative_start_curve_at_last_parameter.Y() + d_vec_curve_start.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Z(), last_point_start_curve.Z() + 2 * first_derivative_start_curve_at_last_parameter.Z() + d_vec_curve_start.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_5)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G0);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G2);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 4);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    gp_Vec d_vec_curve_end = displacement_of_third_control_point(bezier_curve->Degree(), 1., 1., first_derivative_end_curve_at_first_parameter, second_derivative_end_curve_at_first_parameter);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), first_point_end_curve.X() - 2 * first_derivative_end_curve_at_first_parameter.X() + d_vec_curve_end.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), first_point_end_curve.Y() - 2 * first_derivative_end_curve_at_first_parameter.Y() + d_vec_curve_end.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), first_point_end_curve.Z() - 2 * first_derivative_end_curve_at_first_parameter.Z() + d_vec_curve_end.Z(), 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(3).X(), first_point_end_curve.X() - first_derivative_end_curve_at_first_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Y(), first_point_end_curve.Y() - first_derivative_end_curve_at_first_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Z(), first_point_end_curve.Z() - first_derivative_end_curve_at_first_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_6)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G2);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G1);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 5);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), last_point_start_curve.X() + first_derivative_start_curve_at_last_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), last_point_start_curve.Y() + first_derivative_start_curve_at_last_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), last_point_start_curve.Z() + first_derivative_start_curve_at_last_parameter.Z(), 1e-5);

    gp_Vec d_vec_curve_start = displacement_of_third_control_point(bezier_curve->Degree(), 1., 1., first_derivative_start_curve_at_last_parameter, second_derivative_start_curve_at_last_parameter);

    EXPECT_NEAR(bezier_curve->Pole(3).X(), last_point_start_curve.X() + 2 * first_derivative_start_curve_at_last_parameter.X() + d_vec_curve_start.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Y(), last_point_start_curve.Y() + 2 * first_derivative_start_curve_at_last_parameter.Y() + d_vec_curve_start.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Z(), last_point_start_curve.Z() + 2 * first_derivative_start_curve_at_last_parameter.Z() + d_vec_curve_start.Z(), 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(4).X(), first_point_end_curve.X() - first_derivative_end_curve_at_first_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(4).Y(), first_point_end_curve.Y() - first_derivative_end_curve_at_first_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(4).Z(), first_point_end_curve.Z() - first_derivative_end_curve_at_first_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_7)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G1);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G2);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 5);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), last_point_start_curve.X() + first_derivative_start_curve_at_last_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), last_point_start_curve.Y() + first_derivative_start_curve_at_last_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), last_point_start_curve.Z() + first_derivative_start_curve_at_last_parameter.Z(), 1e-5);

    gp_Vec d_vec_curve_end = displacement_of_third_control_point(bezier_curve->Degree(), 1., 1., first_derivative_end_curve_at_first_parameter, second_derivative_end_curve_at_first_parameter);

    EXPECT_NEAR(bezier_curve->Pole(3).X(), first_point_end_curve.X() - 2 * first_derivative_end_curve_at_first_parameter.X() + d_vec_curve_end.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Y(), first_point_end_curve.Y() - 2 * first_derivative_end_curve_at_first_parameter.Y() + d_vec_curve_end.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Z(), first_point_end_curve.Z() - 2 * first_derivative_end_curve_at_first_parameter.Z() + d_vec_curve_end.Z(), 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(4).X(), first_point_end_curve.X() - first_derivative_end_curve_at_first_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(4).Y(), first_point_end_curve.Y() - first_derivative_end_curve_at_first_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(4).Z(), first_point_end_curve.Z() - first_derivative_end_curve_at_first_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_8)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G2);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G2);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 6);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), last_point_start_curve.X() + first_derivative_start_curve_at_last_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), last_point_start_curve.Y() + first_derivative_start_curve_at_last_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), last_point_start_curve.Z() + first_derivative_start_curve_at_last_parameter.Z(), 1e-5);

    gp_Vec d_vec_curve_start = displacement_of_third_control_point(bezier_curve->Degree(), 1., 1., first_derivative_start_curve_at_last_parameter, second_derivative_start_curve_at_last_parameter);

    EXPECT_NEAR(bezier_curve->Pole(3).X(), last_point_start_curve.X() + 2 * first_derivative_start_curve_at_last_parameter.X() + d_vec_curve_start.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Y(), last_point_start_curve.Y() + 2 * first_derivative_start_curve_at_last_parameter.Y() + d_vec_curve_start.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Z(), last_point_start_curve.Z() + 2 * first_derivative_start_curve_at_last_parameter.Z() + d_vec_curve_start.Z(), 1e-5);

    gp_Vec d_vec_curve_end = displacement_of_third_control_point(bezier_curve->Degree(), 1., 1., first_derivative_end_curve_at_first_parameter, second_derivative_end_curve_at_first_parameter);

    EXPECT_NEAR(bezier_curve->Pole(4).X(), first_point_end_curve.X() - 2 * first_derivative_end_curve_at_first_parameter.X() + d_vec_curve_end.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(4).Y(), first_point_end_curve.Y() - 2 * first_derivative_end_curve_at_first_parameter.Y() + d_vec_curve_end.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(4).Z(), first_point_end_curve.Z() - 2 * first_derivative_end_curve_at_first_parameter.Z() + d_vec_curve_end.Z(), 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(5).X(), first_point_end_curve.X() - first_derivative_end_curve_at_first_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(5).Y(), first_point_end_curve.Y() - first_derivative_end_curve_at_first_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(5).Z(), first_point_end_curve.Z() - first_derivative_end_curve_at_first_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_9)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G1, false);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G0);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 3);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), last_point_start_curve.X() - first_derivative_start_curve_at_last_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), last_point_start_curve.Y() - first_derivative_start_curve_at_last_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), last_point_start_curve.Z() - first_derivative_start_curve_at_last_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_10)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G0);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G1, false);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 3);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), first_point_end_curve.X() + first_derivative_end_curve_at_first_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), first_point_end_curve.Y() + first_derivative_end_curve_at_first_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), first_point_end_curve.Z() + first_derivative_end_curve_at_first_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_11)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G2, false);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G0);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 4);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), last_point_start_curve.X() - first_derivative_start_curve_at_last_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), last_point_start_curve.Y() - first_derivative_start_curve_at_last_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), last_point_start_curve.Z() - first_derivative_start_curve_at_last_parameter.Z(), 1e-5);

    gp_Vec d_vec_curve_start = displacement_of_third_control_point(bezier_curve->Degree(), 1., 1., first_derivative_start_curve_at_last_parameter, second_derivative_start_curve_at_last_parameter);

    EXPECT_NEAR(bezier_curve->Pole(3).X(), last_point_start_curve.X() - 2 * first_derivative_start_curve_at_last_parameter.X() + d_vec_curve_start.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Y(), last_point_start_curve.Y() - 2 * first_derivative_start_curve_at_last_parameter.Y() + d_vec_curve_start.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Z(), last_point_start_curve.Z() - 2 * first_derivative_start_curve_at_last_parameter.Z() + d_vec_curve_start.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_12)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G0);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G2, false);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 4);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    gp_Vec d_vec_curve_end = displacement_of_third_control_point(bezier_curve->Degree(), 1., 1., first_derivative_end_curve_at_first_parameter, second_derivative_end_curve_at_first_parameter);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), first_point_end_curve.X() + 2 * first_derivative_end_curve_at_first_parameter.X() + d_vec_curve_end.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), first_point_end_curve.Y() + 2 * first_derivative_end_curve_at_first_parameter.Y() + d_vec_curve_end.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), first_point_end_curve.Z() + 2 * first_derivative_end_curve_at_first_parameter.Z() + d_vec_curve_end.Z(), 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(3).X(), first_point_end_curve.X() + first_derivative_end_curve_at_first_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Y(), first_point_end_curve.Y() + first_derivative_end_curve_at_first_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Z(), first_point_end_curve.Z() + first_derivative_end_curve_at_first_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_13)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G2, false);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G2, false);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 6);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), last_point_start_curve.X() - first_derivative_start_curve_at_last_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), last_point_start_curve.Y() - first_derivative_start_curve_at_last_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), last_point_start_curve.Z() - first_derivative_start_curve_at_last_parameter.Z(), 1e-5);

    gp_Vec d_vec_curve_start = displacement_of_third_control_point(bezier_curve->Degree(), 1., 1., first_derivative_start_curve_at_last_parameter, second_derivative_start_curve_at_last_parameter);

    EXPECT_NEAR(bezier_curve->Pole(3).X(), last_point_start_curve.X() - 2 * first_derivative_start_curve_at_last_parameter.X() + d_vec_curve_start.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Y(), last_point_start_curve.Y() - 2 * first_derivative_start_curve_at_last_parameter.Y() + d_vec_curve_start.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Z(), last_point_start_curve.Z() - 2 * first_derivative_start_curve_at_last_parameter.Z() + d_vec_curve_start.Z(), 1e-5);

    gp_Vec d_vec_curve_end = displacement_of_third_control_point(bezier_curve->Degree(), 1., 1., first_derivative_end_curve_at_first_parameter, second_derivative_end_curve_at_first_parameter);

    EXPECT_NEAR(bezier_curve->Pole(4).X(), first_point_end_curve.X() + 2 * first_derivative_end_curve_at_first_parameter.X() + d_vec_curve_end.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(4).Y(), first_point_end_curve.Y() + 2 * first_derivative_end_curve_at_first_parameter.Y() + d_vec_curve_end.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(4).Z(), first_point_end_curve.Z() + 2 * first_derivative_end_curve_at_first_parameter.Z() + d_vec_curve_end.Z(), 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(5).X(), first_point_end_curve.X() + first_derivative_end_curve_at_first_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(5).Y(), first_point_end_curve.Y() + first_derivative_end_curve_at_first_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(5).Z(), first_point_end_curve.Z() + first_derivative_end_curve_at_first_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_14)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G1, true, 0.5);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G0);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 3);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), last_point_start_curve.X() + 0.5 * first_derivative_start_curve_at_last_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), last_point_start_curve.Y() + 0.5 * first_derivative_start_curve_at_last_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), last_point_start_curve.Z() + 0.5 * first_derivative_start_curve_at_last_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_15)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G0);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G1, true, 0.5);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 3);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), first_point_end_curve.X() - 0.5 * first_derivative_end_curve_at_first_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), first_point_end_curve.Y() - 0.5 * first_derivative_end_curve_at_first_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), first_point_end_curve.Z() - 0.5 * first_derivative_end_curve_at_first_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_16)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G2, true, 0.5, 2.);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G0);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 4);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), last_point_start_curve.X() + 0.5 * first_derivative_start_curve_at_last_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), last_point_start_curve.Y() + 0.5 * first_derivative_start_curve_at_last_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), last_point_start_curve.Z() + 0.5 * first_derivative_start_curve_at_last_parameter.Z(), 1e-5);

    gp_Vec d_vec_curve_end = displacement_of_third_control_point(bezier_curve->Degree(), 0.5, 2., first_derivative_start_curve_at_last_parameter, second_derivative_start_curve_at_last_parameter);

    EXPECT_NEAR(bezier_curve->Pole(3).X(), last_point_start_curve.X() + 2 * 0.5 * 2 * first_derivative_start_curve_at_last_parameter.X() + d_vec_curve_end.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Y(), last_point_start_curve.Y() + 2 * 0.5 * 2 * first_derivative_start_curve_at_last_parameter.Y() + d_vec_curve_end.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Z(), last_point_start_curve.Z() + 2 * 0.5 * 2 * first_derivative_start_curve_at_last_parameter.Z() + d_vec_curve_end.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_17)
{
    geoml::BlendCurveConnection start_connection (geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G0);
    geoml::BlendCurveConnection end_connection (geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G2, true, 0.5, 2.);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 4);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    gp_Vec d_vec_curve_end = displacement_of_third_control_point(bezier_curve->Degree(), 0.5, 2., first_derivative_end_curve_at_first_parameter, second_derivative_end_curve_at_first_parameter);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), first_point_end_curve.X() - 2 * 0.5 * 2 * first_derivative_end_curve_at_first_parameter.X() + d_vec_curve_end.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), first_point_end_curve.Y() - 2 * 0.5 * 2 * first_derivative_end_curve_at_first_parameter.Y() + d_vec_curve_end.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), first_point_end_curve.Z() - 2 * 0.5 * 2 * first_derivative_end_curve_at_first_parameter.Z() + d_vec_curve_end.Z(), 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(3).X(), first_point_end_curve.X() - 0.5 * first_derivative_end_curve_at_first_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Y(), first_point_end_curve.Y() - 0.5 * first_derivative_end_curve_at_first_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Z(), first_point_end_curve.Z() - 0.5 * first_derivative_end_curve_at_first_parameter.Z(), 1e-5);
}

TEST_F(CurvesToBlend, blend_curve_test_18)
{
    geoml::BlendCurveConnection start_connection(geoml::CurveToEdge(start_curve), cp_start_3, geoml::GContinuity::G2, true, 0.25);
    geoml::BlendCurveConnection end_connection(geoml::CurveToEdge(end_curve), cp_end_1, geoml::GContinuity::G2, true, 1., 2.);

    TopoDS_Edge resulting_blend_curve = geoml::blend_curve(start_connection, end_connection);

    Handle(Geom_Curve) curve = geoml::EdgeToCurve(resulting_blend_curve);

    Handle(Geom_BezierCurve) bezier_curve = Handle(Geom_BezierCurve)::DownCast(curve);

    EXPECT_EQ(bezier_curve->NbPoles(), 6);

    EXPECT_NEAR(bezier_curve->StartPoint().X(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Y(), 2., 1e-5);
    EXPECT_NEAR(bezier_curve->StartPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->EndPoint().X(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Y(), 4., 1e-5);
    EXPECT_NEAR(bezier_curve->EndPoint().Z(), 0., 1e-5);

    EXPECT_NEAR(bezier_curve->Pole(2).X(), last_point_start_curve.X() + 0.25 * first_derivative_start_curve_at_last_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Y(), last_point_start_curve.Y() + 0.25 * first_derivative_start_curve_at_last_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(2).Z(), last_point_start_curve.Z() + 0.25 * first_derivative_start_curve_at_last_parameter.Z(), 1e-5);

    gp_Vec d_vec_curve_start = displacement_of_third_control_point(bezier_curve->Degree(), 0.25, 1., first_derivative_start_curve_at_last_parameter, second_derivative_start_curve_at_last_parameter);

    EXPECT_NEAR(bezier_curve->Pole(3).X(), last_point_start_curve.X() + 0.25 * 2 * first_derivative_start_curve_at_last_parameter.X() + d_vec_curve_start.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Y(), last_point_start_curve.Y() + 0.25 * 2 * first_derivative_start_curve_at_last_parameter.Y() + d_vec_curve_start.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(3).Z(), last_point_start_curve.Z() + 0.25 * 2 * first_derivative_start_curve_at_last_parameter.Z() + d_vec_curve_start.Z(), 1e-5);

    gp_Vec d_vec_curve_end = displacement_of_third_control_point(bezier_curve->Degree(), 1., 2., first_derivative_end_curve_at_first_parameter, second_derivative_end_curve_at_first_parameter);

    EXPECT_NEAR(bezier_curve->Pole(4).X(), first_point_end_curve.X() - 2 * 2 * first_derivative_end_curve_at_first_parameter.X() + d_vec_curve_end.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(4).Y(), first_point_end_curve.Y() - 2 * 2 * first_derivative_end_curve_at_first_parameter.Y() + d_vec_curve_end.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(4).Z(), first_point_end_curve.Z() - 2 * 2 * first_derivative_end_curve_at_first_parameter.Z() + d_vec_curve_end.Z(), 1e-5);
   
    EXPECT_NEAR(bezier_curve->Pole(5).X(), first_point_end_curve.X() - first_derivative_end_curve_at_first_parameter.X(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(5).Y(), first_point_end_curve.Y() - first_derivative_end_curve_at_first_parameter.Y(), 1e-5);
    EXPECT_NEAR(bezier_curve->Pole(5).Z(), first_point_end_curve.Z() - first_derivative_end_curve_at_first_parameter.Z(), 1e-5);
}

