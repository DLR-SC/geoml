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

TEST(SimpleCurveTest, closed_unclamped_nurbs_curve)
{    

// define a closed B-spline curve of 
// degree 2 by overlapping two control points (same number as the degree) and 
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

// knots (uniform but not quasi-uniform, i.e. the B-spline curve is not clamped):
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

}
