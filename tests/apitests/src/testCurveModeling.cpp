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
#include "STEPControl_Writer.hxx"

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

TColgp_Array1OfPnt control_points(1,5);

control_points.SetValue(1,p_1);
control_points.SetValue(2,p_2);
control_points.SetValue(3,p_3);
control_points.SetValue(4,p_4);
control_points.SetValue(5,p_5);

// degree:
Standard_Integer degree = 2;

// weights (non-rational):
TColStd_Array1OfReal weights(1, 5);
weights.SetValue(1, 1.0);  
weights.SetValue(2, 1.0);  
weights.SetValue(3, 1.0);  
weights.SetValue(4, 1.0);
weights.SetValue(5, 1.0);

// knots: note: as the number of knots (m+1), the number of control points (p+1) and the degree p
// are related by m = n + p +1, the number of knots has to be 8 in this case.
TColStd_Array1OfReal knots(1,8); 
knots.SetValue(1,0.0);           
knots.SetValue(2,1.0);
knots.SetValue(3,2.0);
knots.SetValue(4,3.0);           
knots.SetValue(5,4.0);
knots.SetValue(6,5.0);
knots.SetValue(7,6.0);           
knots.SetValue(8,7.0);

// multiplicities: 
TColStd_Array1OfInteger mults(1,8); 
mults.SetValue(1,1);               
mults.SetValue(2,1);
mults.SetValue(3,1);
mults.SetValue(4,1);
mults.SetValue(5,1);
mults.SetValue(6,1);
mults.SetValue(7,1);
mults.SetValue(8,1);

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

TColgp_Array1OfPnt control_points(1,5);

control_points.SetValue(1,p1);
control_points.SetValue(2,p2);
control_points.SetValue(3,p3);
control_points.SetValue(4,p4);
control_points.SetValue(5,p5);  

// degree:
Standard_Integer degree = 2;

// weights:
TColStd_Array1OfReal weights(1, 5);
weights.SetValue(1, 1.0);  
weights.SetValue(2, 1.0);  
weights.SetValue(3, 1.0);  
weights.SetValue(4, 1.0);
weights.SetValue(5, 1.0);

// knots: note: as the number of knots (m+1), the number of control points (p+1) and the degree p
// are related by m = n + p +1, the number of knots has to be 8 in this case. To achive this, in the following the
// first and the last knot have to appear trice (have multiplicity of 3)
TColStd_Array1OfReal knots(1,4); 
knots.SetValue(1,0.0);           
knots.SetValue(2,1.0);
knots.SetValue(3,2.0);
knots.SetValue(4,3.0);

// multiplicities: 
TColStd_Array1OfInteger mults(1,4); 
mults.SetValue(1,3);                
mults.SetValue(2,1);
mults.SetValue(3,1);
mults.SetValue(4,3);

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

    writeToStp(curve, "clamped_Nurbs.stp");
}

