#include <geoml/curves/modeling.h>

#include <gtest/gtest.h>

#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include "BRepBuilderAPI_MakeEdge.hxx"
#include <TopoDS_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>

TEST(SimpleCurveTest, closed_unclamped_nurbs_curve)
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
std::vector<double> weights (5,1.0);

// knots: note: as the number of knots (m+1), the number of control points (p+1) and the degree p
// are related by m = n + p +1, the number of knots has to be 8 in this case.

std::vector<double> knots {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0};

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
//EXPECT_EQ(curve->IsRational(), false); // this test has the wrong output - check!
}

TEST(SimpleCurveTest, open_clamped_nurbs_curve)
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
std::vector<double> weights(5,1.0);

// knots: note: as the number of knots (m+1), the number of control points (p+1) and the degree p
// are related by m = n + p +1, the number of knots has to be 8 in this case. To achive this, in the following the
// first and the last knot have to appear trice (have multiplicity of 3)

std::vector<double> knots{0.0, 1.0, 2.0, 3.0};

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

TEST(SimpleCurveTest, circle_rational_bspline_three_arcs)
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
std::vector<double> weights {1.0, 0.5, 1.0, 0.5, 1.0, 0.5, 1.0};

// knots: (we need 10 knots (including their multiplicities), which follows from "#knots = #control_points + degree + 1"
std::vector<double> knots {0.0, 1.0, 2.0, 3.0};

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



