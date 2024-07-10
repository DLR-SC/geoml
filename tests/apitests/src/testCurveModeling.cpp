#include <geoml/curves/modeling.h>
#include "geometry/BSplineAlgorithms.h"

#include <gtest/gtest.h>

#include <gp_Pnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include "BRepBuilderAPI_MakeEdge.hxx"
#include <TopoDS_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>

/////////
#include "STEPControl_Writer.hxx"
#include <BRepBuilderAPI_MakeFace.hxx>

void writeToStp(Handle_Geom_Curve cur, std::string const& name_file)
{
    BRepBuilderAPI_MakeEdge edgeMaker;
    edgeMaker.Init(cur);
    TopoDS_Shape edge = edgeMaker.Shape();

    STEPControl_Writer aStepWriter;
    aStepWriter.Transfer(edge,STEPControl_AsIs);
    aStepWriter.Write(name_file.c_str());
} 

void writeGeomEntityToStepFile(Handle_Geom_Surface surface, std::string fileName)
{
	BRepBuilderAPI_MakeFace faceMaker;
	faceMaker.Init(surface,true,1E-6);	
	TopoDS_Shape face = faceMaker.Shape();

	STEPControl_Writer writer;
	writer.Transfer(face,STEPControl_AsIs);
	writer.Write(fileName.c_str());
	return;
}

void writeGeomEntityToStepFile(const TopoDS_Shape& my_shape, std::string fileName)
{
    STEPControl_Writer writer;
    writer.Transfer(my_shape,STEPControl_AsIs);
    writer.Write(fileName.c_str());
}
/////////


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

TEST(Test_curve_blend_bezier, simple_curve_blend_test)
{
    // create two curves which will be connected by a blending curve

    // curve 1
    gp_Pnt crv_1_pt_1 (0.0, 0.0, 0.0);
    gp_Pnt crv_1_pt_2 (1.0, 0.0, 0.0);
    gp_Pnt crv_1_pt_3 (2.0, 0.0, 1.0);
    gp_Pnt crv_1_pt_4 (6.0, 0.0, 0.0);

    std::vector <gp_Pnt> input_points_1 {crv_1_pt_1, crv_1_pt_2, crv_1_pt_3, crv_1_pt_4};

    Handle(Geom_BSplineCurve) curve_1 = geoml::interpolate_points_to_b_spline_curve(input_points_1, 3, false);

    gp_Pnt pnt_curve_1;
    gp_Vec derivative_1_curve_1;
    gp_Vec derivative_2_curve_1;

    // compute the end point of curve_1 and first and second derivative at this point:
    curve_1->D2(1., pnt_curve_1, derivative_1_curve_1, derivative_2_curve_1);

    // curve 2
    gp_Pnt crv_2_pt_1 (9.0, 0.0, 0.0);
    gp_Pnt crv_2_pt_2 (10.0, 0.0, 0.0);
    gp_Pnt crv_2_pt_3 (11.0, 0.0, 1.0);
    gp_Pnt crv_2_pt_4 (15.0, 0.0, 0.0);

    std::vector <gp_Pnt> input_points_2 {crv_2_pt_1, crv_2_pt_2, crv_2_pt_3, crv_2_pt_4};

    Handle(Geom_BSplineCurve) curve_2 = geoml::interpolate_points_to_b_spline_curve(input_points_2, 3, false);

    // continuity: G_0, G_0:

    Handle(Geom_BSplineCurve) blending_crv_1 = geoml::blend_curve(curve_1, 
                                                                curve_2,   
                                                                1, //start_end_1
                                                                0, //start_end_2
                                                                0, //continuity_1
                                                                0, //continuity_2
                                                                1., //form_factor_11
                                                                1., //form_factor_12
                                                                1., //form_factor_21
                                                                1.); //form_factor_22

    EXPECT_NEAR(blending_crv_1->FirstParameter(), 0.0, 1e-5);
    EXPECT_NEAR(blending_crv_1->LastParameter(), 1.0, 1e-5);

    EXPECT_NEAR(blending_crv_1->StartPoint().X(), crv_1_pt_4.X(), 1e-5);
    EXPECT_NEAR(blending_crv_1->StartPoint().Y(), crv_1_pt_4.Y(), 1e-5);
    EXPECT_NEAR(blending_crv_1->StartPoint().Z(), crv_1_pt_4.Z(), 1e-5);

    EXPECT_NEAR(blending_crv_1->EndPoint().X(), crv_2_pt_1.X(), 1e-5);
    EXPECT_NEAR(blending_crv_1->EndPoint().Y(), crv_2_pt_1.Y(), 1e-5);
    EXPECT_NEAR(blending_crv_1->EndPoint().Z(), crv_2_pt_1.Z(), 1e-5);

    // continuity: G_1, G_1:

        Handle(Geom_BSplineCurve) blending_crv_2 = geoml::blend_curve(curve_1, 
                                                                curve_2,   
                                                                1, //start_end_1
                                                                0, //start_end_2
                                                                1, //continuity_1
                                                                1, //continuity_2
                                                                2., //form_factor_11
                                                                2., //form_factor_12
                                                                2., //form_factor_21
                                                                2.); //form_factor_22

    EXPECT_NEAR(blending_crv_2->FirstParameter(), 0.0, 1e-5);
    EXPECT_NEAR(blending_crv_2->LastParameter(), 1.0, 1e-5);

    Standard_Real formfactor_blending_curve_2_11 (2.);

    gp_Pnt tangentially_translated_start_point_of_curve_1_bc_2 = pnt_curve_1.Translated(formfactor_blending_curve_2_11 / 3 * derivative_1_curve_1); 

    EXPECT_NEAR(blending_crv_2->Pole(2).X(), tangentially_translated_start_point_of_curve_1_bc_2.X(), 1e-5);
    EXPECT_NEAR(blending_crv_2->Pole(2).Y(), tangentially_translated_start_point_of_curve_1_bc_2.Y(), 1e-5);
    EXPECT_NEAR(blending_crv_2->Pole(2).Z(), tangentially_translated_start_point_of_curve_1_bc_2.Z(), 1e-5);

    // continuity: G_2, G_2:

        Handle(Geom_BSplineCurve) blending_crv_3 = geoml::blend_curve(curve_1, 
                                                                curve_2,   
                                                                1, //start_end_1
                                                                0, //start_end_2
                                                                2, //continuity_1
                                                                2, //continuity_2
                                                                0.2, //form_factor_11
                                                                0.2, //form_factor_12
                                                                0.2, //form_factor_21
                                                                0.2); //form_factor_22

    EXPECT_NEAR(blending_crv_3->FirstParameter(), 0.0, 1e-5);
    EXPECT_NEAR(blending_crv_3->LastParameter(), 1.0, 1e-5);

    Standard_Real formfactor_blending_curve_3_11 (0.2);
    Standard_Real formfactor_blending_curve_3_21 (0.2);

    gp_Pnt tangentially_translated_start_point_of_curve_1_bc_3 = pnt_curve_1.Translated(formfactor_blending_curve_3_11 / 3 * derivative_1_curve_1); 

    EXPECT_NEAR(blending_crv_3->Pole(2).X(), tangentially_translated_start_point_of_curve_1_bc_3.X(), 1e-5);
    EXPECT_NEAR(blending_crv_3->Pole(2).Y(), tangentially_translated_start_point_of_curve_1_bc_3.Y(), 1e-5);
    EXPECT_NEAR(blending_crv_3->Pole(2).Z(), tangentially_translated_start_point_of_curve_1_bc_3.Z(), 1e-5);

    // compute the curvature point at start and compare the third contorl point coincides with the computation:
    gp_Vec trans_vec_bc_3 = formfactor_blending_curve_3_11 / 6 * derivative_2_curve_1 + (formfactor_blending_curve_3_21 / 6 + 2 / 3 * formfactor_blending_curve_3_11) * derivative_1_curve_1;
                                                
    gp_Pnt computed_curvature_point_start_bc_3 = pnt_curve_1.Translated(trans_vec_bc_3);

    EXPECT_NEAR(blending_crv_3->Pole(3).X(), computed_curvature_point_start_bc_3.X(), 1e-5);
    EXPECT_NEAR(blending_crv_3->Pole(3).Y(), computed_curvature_point_start_bc_3.Y(), 1e-5);
    EXPECT_NEAR(blending_crv_3->Pole(3).Z(), computed_curvature_point_start_bc_3.Z(), 1e-5);

}



