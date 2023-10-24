//#include <geoml/surfaces/modeling.h>
#include <geoml/curves/modeling.h>

#include <gtest/gtest.h>

#include <GeomConvert.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

#include <TopoDS_Shape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include <filesystem>
#include <iostream>
#include <cmath>
#include <vector>
#include <geoml/surfaces/modeling.h>
#include <gp_Pnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include "Geom_Curve.hxx"
#include <GeomAPI_PointsToBSpline.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GeomAdaptor_Curve.hxx>
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "STEPControl_Writer.hxx"

/*
namespace apitests
{

}
*/

TEST(simple_curve_test, closed_unclamped_b_spline)
{    

// define a closed b-spline curve of 
// degree 2 by overlapping two control points and 
// a uniform choice of knots      

// define control points
gp_Pnt p_11(0.0, 0.0, 0.0);
gp_Pnt p_12(0.0, 1.0, 2.0);
gp_Pnt p_13(0.0, 2.0, -1.0);
gp_Pnt p_14(0.0, 0.0, 0.0);
gp_Pnt p_15(0.0, 1.0, 2.0);

TColgp_Array1OfPnt control_points_1(1,5);

control_points_1.SetValue(1,p_11);
control_points_1.SetValue(2,p_12);
control_points_1.SetValue(3,p_13);
control_points_1.SetValue(4,p_14);
control_points_1.SetValue(5,p_15);

// degree:
Standard_Integer degree_1 = 2;

// weights (non-rational):
TColStd_Array1OfReal weights_1(1, 5);
weights_1.SetValue(1, 1.0);  
weights_1.SetValue(2, 1.0);  
weights_1.SetValue(3, 1.0);  
weights_1.SetValue(4, 1.0);
weights_1.SetValue(5, 1.0);

// knots (uniform but not quasi-uniform, i.e. the b-spline curve is not clamped):
TColStd_Array1OfReal knots_1(1,8); 
knots_1.SetValue(1,0.0);           
knots_1.SetValue(2,1.0);
knots_1.SetValue(3,2.0);
knots_1.SetValue(4,3.0);           
knots_1.SetValue(5,4.0);
knots_1.SetValue(6,5.0);
knots_1.SetValue(7,6.0);           
knots_1.SetValue(8,7.0);

// multiplicities: 
TColStd_Array1OfInteger mults_1(1,8); 
mults_1.SetValue(1,1);               
mults_1.SetValue(2,1);
mults_1.SetValue(3,1);
mults_1.SetValue(4,1);
mults_1.SetValue(5,1);
mults_1.SetValue(6,1);
mults_1.SetValue(7,1);
mults_1.SetValue(8,1);

Handle(Geom_BSplineCurve) my_curve =
    geoml::b_spline_curve(
        control_points_1, 
        weights_1, 
        knots_1, 
        mults_1, 
        degree_1);

    EXPECT_NEAR(
        my_curve->StartPoint().X(), 
        my_curve->EndPoint().X(), 
        1e-5);

    EXPECT_NEAR(
        my_curve->StartPoint().Y(), 
        my_curve->EndPoint().Y(), 
        1e-5);
    
    EXPECT_NEAR(
        my_curve->StartPoint().Z(), 
        my_curve->EndPoint().Z(), 
        1e-5);
}

TEST(simple_curve_test, open_clamped_b_spline_curve)
{

// define an open and clamped B-spline curve of degree 2

// control points
gp_Pnt p_11(0.0, 0.0, 0.0);
gp_Pnt p_12(0.0, 1.0, 2.0);
gp_Pnt p_13(0.0, 2.0, -1.0);
gp_Pnt p_14(1.0, 0.0, 3.0);
gp_Pnt p_15(0.0, 4.0, 0.0);

TColgp_Array1OfPnt control_points_1(1,5);

control_points_1.SetValue(1,p_11);
control_points_1.SetValue(2,p_12);
control_points_1.SetValue(3,p_13);
control_points_1.SetValue(4,p_14);
control_points_1.SetValue(5,p_15);  

// degree:
Standard_Integer degree_1 = 2;

// weights:
TColStd_Array1OfReal weights_1(1, 5);
weights_1.SetValue(1, 1.0);  
weights_1.SetValue(2, 1.0);  
weights_1.SetValue(3, 1.0);  
weights_1.SetValue(4, 1.0);
weights_1.SetValue(5, 1.0);

//knots:
TColStd_Array1OfReal knots_1(1,4); 
knots_1.SetValue(1,0.0);           
knots_1.SetValue(2,1.0);
knots_1.SetValue(3,2.0);
knots_1.SetValue(4,3.0);

// multiplicities: 
TColStd_Array1OfInteger mults_1(1,4); 
mults_1.SetValue(1,3);                
mults_1.SetValue(2,1);
mults_1.SetValue(3,1);
mults_1.SetValue(4,3);

Handle(Geom_BSplineCurve) my_curve =
    geoml::b_spline_curve(
        control_points_1, 
        weights_1, 
        knots_1, 
        mults_1, 
        degree_1);

    EXPECT_NEAR(
        my_curve->StartPoint().X(), 
        p_11.X(), 
        1e-5);

    EXPECT_NEAR(
        my_curve->StartPoint().Y(), 
        p_11.Y(), 
        1e-5);
    
    EXPECT_NEAR(
        my_curve->StartPoint().Z(), 
        p_11.Z(), 
        1e-5);

    EXPECT_NEAR(
        my_curve->EndPoint().X(), 
        p_15.X(), 
        1e-5);

    EXPECT_NEAR(
        my_curve->EndPoint().Y(), 
        p_15.Y(), 
        1e-5);
    
    EXPECT_NEAR(
        my_curve->EndPoint().Z(), 
        p_15.Z(), 
        1e-5);
}


// define a function that writes curves to step files:
void writeToStp(Handle_Geom_Curve cur, std::string const& name_file)
{
BRepBuilderAPI_MakeEdge edgeMaker;
edgeMaker.Init(cur);
TopoDS_Shape edge = edgeMaker.Shape();

STEPControl_Writer aStepWriter;
aStepWriter.Transfer(edge,STEPControl_AsIs);
aStepWriter.Write(name_file.c_str());
} 


TEST(simple_curve_test, circle_rational_bspline_three_arcs)
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

TColgp_Array1OfPnt control_points(1,7);

control_points.SetValue(1,p_1);
control_points.SetValue(2,p_2);
control_points.SetValue(3,p_3);
control_points.SetValue(4,p_4);
control_points.SetValue(5,p_5);  
control_points.SetValue(6,p_6);
control_points.SetValue(7,p_7);  
  
// degree:
Standard_Integer degree = 2;

// weights:
TColStd_Array1OfReal weights(1, 7);
weights.SetValue(1, 1.0);  
weights.SetValue(2, 0.5);  
weights.SetValue(3, 1.0);  
weights.SetValue(4, 0.5);
weights.SetValue(5, 1.0);
weights.SetValue(6, 0.5);
weights.SetValue(7, 1.0);

//knots (m+1) = (n+1) + p + 1:
TColStd_Array1OfReal knots(1,4); 
knots.SetValue(1,0.0);           
knots.SetValue(2,1.0);
knots.SetValue(3,2.0);
knots.SetValue(4,3.0);

// multiplicities: 
TColStd_Array1OfInteger mults(1,4); 
mults.SetValue(1,3);                
mults.SetValue(2,2);
mults.SetValue(3,2);
mults.SetValue(4,3);

Handle(Geom_BSplineCurve) my_curve =
    geoml::b_spline_curve(
        control_points, 
        weights, 
        knots, 
        mults, 
        degree);

// get the length of my_curve
Standard_Real umin = my_curve->FirstParameter();
Standard_Real umax = my_curve->LastParameter();
GeomAdaptor_Curve adaptorCurve(my_curve, umin, umax);
Standard_Real length = GCPnts_AbscissaPoint::Length(adaptorCurve, umin, umax);

// Expect the circumference of the circle.
EXPECT_NEAR(length, 12.566, 1e-3);
EXPECT_EQ(my_curve->NbPoles(), 7);
EXPECT_EQ(my_curve->NbKnots(), 4);
EXPECT_FALSE(my_curve->IsPeriodic());

EXPECT_EQ(my_curve->Multiplicity(1), 3);
EXPECT_EQ(my_curve->Multiplicity(2), 2);
EXPECT_EQ(my_curve->Multiplicity(3), 2);
EXPECT_EQ(my_curve->Multiplicity(4), 3);

my_curve->SetPeriodic();

EXPECT_TRUE(my_curve->IsPeriodic());
EXPECT_EQ(my_curve->NbPoles(), 6);
EXPECT_EQ(my_curve->NbKnots(), 4);

EXPECT_EQ(my_curve->Multiplicity(1), 2);
EXPECT_EQ(my_curve->Multiplicity(2), 2);
EXPECT_EQ(my_curve->Multiplicity(3), 2);
EXPECT_EQ(my_curve->Multiplicity(4), 2);

}