#include <iostream>
#include <cmath>
#include <vector>

#include <geoml/curves/modeling.h>
#include <geoml/surfaces/modeling.h>

#include <gp_Pnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include "Geom_Curve.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include <TopoDS_Shape.hxx>
#include "STEPControl_Writer.hxx"

#include <GeomAPI_PointsToBSpline.hxx>
//#include <NCollection_Array1.hxx> // for using TColgp_Array1OfPnt
#include <TColgp_Array1OfPnt.hxx>


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

int main(){
/*
/////////////////////////////////////////////////////
//// interpolate NACA points to a B-spline curve: ////
/////////////////////////////////////////////////////

// list of 21 points on NACA2412 profile:
std::vector<double> px {1.000084, 0.975825, 0.905287, 0.795069, 0.655665, 0.500588, 0.34468, 0.203313, 0.091996, 0.022051, 0.0, 0.026892, 0.098987, 0.208902, 0.346303, 0.499412, 0.653352, 0.792716, 0.90373, 0.975232, 0.999916};
std::vector<double> pz {0.001257, 0.006231, 0.019752, 0.03826, 0.057302, 0.072381, 0.079198, 0.072947, 0.054325, 0.028152, 0.0, -0.023408, -0.037507, -0.042346, -0.039941, -0.033493, -0.0245, -0.015499, -0.008033, -0.003035, -0.001257};

// create a curve in y=0 plane:
Handle(TColgp_HArray1OfPnt) points_NACA = new TColgp_HArray1OfPnt(1,21);

for(int i = 1; i<=px.size() ;++i)
{
	points_NACA->SetValue(i,gp_Pnt(px.at(i-1), pz.at(i-1) , 0.0));
}

// interpolate the points with a B-spline curve
Handle(Geom_BSplineCurve) mySplineNACA_Geom_Curve
        = geoml::interpolate_points_to_b_spline_curve(points_NACA);

// write curve to .stp file
writeToStp(mySplineNACA_Geom_Curve, "new_test_interpolation_curve.stp");
*/

/////////////////////////////////////
/////////////////////////////////////
//// fuselage-wing-configuration ////
/////////////////////////////////////
/////////////////////////////////////

/////////////////////////////////////
////  fuselage //////////////////////
/////////////////////////////////////

////// body - cylinder //////////////
/////////////////////////////////////

// profile curves: //////////////////

// define points of the section in the frontal part: // as I realized looking at the knots, 
gp_Pnt point1(7, 2.100102, 0.682364);                // this won't work! 
gp_Pnt point2(7, 1.297934, 1.786453);
gp_Pnt point3(7, 0, 2.208177);
gp_Pnt point4(7, -1.297934, 1.786453);
gp_Pnt point5(7, -2.100102, 0.682364);
gp_Pnt point6(7, -2.100102, -0.682364);
gp_Pnt point7(7, -1.297934, -1.786453);
gp_Pnt point8(7, 0, -2.208177);
gp_Pnt point9(7, 1.297934, -1.786453);
gp_Pnt point10(7, 2.100102, -0.682364);
gp_Pnt point11(7, 2.100102, 0.682364);

TColgp_Array1OfPnt points_fuselage_section_front(1,11);

points_fuselage_section_front.SetValue(1,point1);
points_fuselage_section_front.SetValue(2,point2);
points_fuselage_section_front.SetValue(3,point3);
points_fuselage_section_front.SetValue(4,point4);
points_fuselage_section_front.SetValue(5,point5);
points_fuselage_section_front.SetValue(6,point6);
points_fuselage_section_front.SetValue(7,point7);
points_fuselage_section_front.SetValue(8,point8);
points_fuselage_section_front.SetValue(9,point9);
points_fuselage_section_front.SetValue(10,point10);
points_fuselage_section_front.SetValue(11,point11);

/////////////////////////////////////
// look at a simple example first: //
/////////////////////////////////////

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

// interpolate the points with a B-spline curve // works well
/*
Handle(Geom_BSplineCurve) my_Geom_Curve_1
        = geoml::interpolate_points_to_b_spline_curve(points_1);
*/

/////////////////////////////////////
// another test: ////////////////////
/////////////////////////////////////

// define B-spline (rational):

// control points (TColgp_Array1OfPnt): points_1
// weights (TColStd_Array1OfReal): all = 1 
// knots (TColStd_Array1OfReal): 
// multiplicities (TColStd_Array1OfInteger):
// degree (Standard_Integer):
// periodic (Standard_Boolean): standard false
// check rational (Standard_Boolean): standard true

// degree:
Standard_Integer degree_1 = 2;

// weights:
TColStd_Array1OfReal weights_1(1, 5);
weights_1.SetValue(1, 1.0);  
weights_1.SetValue(2, 1.0);  
weights_1.SetValue(3, 1.0);  
weights_1.SetValue(4, 1.0);
weights_1.SetValue(5, 1.0);

/*
// knots: #knots = #control_points + degree -1
TColStd_Array1OfReal knots_1(1,control_points_1.Length() + degree_1 + 1); // 7 in this case
knots_1.SetValue(1,0.0);        // open uniform knot vector
knots_1.SetValue(2,0.0);
knots_1.SetValue(3,0.0);
knots_1.SetValue(4,1.0);
knots_1.SetValue(5,2.0);
knots_1.SetValue(6,2.0);
knots_1.SetValue(7,2.0);

// multiplicities: 
TColStd_Array1OfInteger mults_1(1, knots_1.Length()); // 7 in this case
mults_1.SetValue(1,3);
mults_1.SetValue(2,3);
mults_1.SetValue(3,3);
mults_1.SetValue(4,1);
mults_1.SetValue(5,3);
mults_1.SetValue(6,3);
mults_1.SetValue(7,3);
*/


//knots:
TColStd_Array1OfReal knots_1(1,4); 
knots_1.SetValue(1,0.0);           
knots_1.SetValue(2,1.0);
knots_1.SetValue(3,2.0);
knots_1.SetValue(4,3.0);

/*
TColStd_Array1OfReal knots_1(1,7); 
knots_1.SetValue(1,-2.0);
knots_1.SetValue(2,-1.0);
knots_1.SetValue(3,0.0);           
knots_1.SetValue(4,1.0);
knots_1.SetValue(5,2.0);
knots_1.SetValue(6,3.0);
knots_1.SetValue(7,4.0);
*/

// multiplicities: 
TColStd_Array1OfInteger mults_1(1,4); 
mults_1.SetValue(1,3);                
mults_1.SetValue(2,1);
mults_1.SetValue(3,1);
mults_1.SetValue(4,3);

/*
TColStd_Array1OfInteger mults_1(1,7); 
mults_1.SetValue(1,1);
mults_1.SetValue(2,1);
mults_1.SetValue(3,1);
mults_1.SetValue(4,1);
mults_1.SetValue(5,1);
mults_1.SetValue(6,1);
mults_1.SetValue(7,1);
*/

std::cout << "This is 111" << std::endl;

Handle(Geom_BSplineCurve) my_Geom_Curve_1 
        = new Geom_BSplineCurve(control_points_1, weights_1, knots_1, mults_1, degree_1);

std::cout << "This is what we got" << std::endl;
// define the B-spline curve:
//      * want that the given points to be the control points of the B-spline curve
//        (i.e. no interpolation)

//int degree(3);
/*
Handle(Geom_BSplineCurve) bSpline_curve 
        = new Geom_BSplineCurve(points_fuselage_section_front, degree);
*/

/*
Handle(Geom_BSplineCurve) fuselage_section_front
        = geoml::interpolate_points_to_b_spline_curve(points_fuselage_section_front);
*/

/*
GeomAPI_PointsToBSpline pointToBSpline(points_fuselage_section_front);
Handle(Geom_BSplineCurve) fuselage_section_front = pointToBSpline.Curve();
*/

/*
Handle(Geom_BSplineCurve) fuselage_section_front = new Geom_BSplineCurve(points_fuselage_section_front, degree);
*/

// write curve to .stp file
writeToStp(my_Geom_Curve_1, "test_curve_1.stp");

return 0;
}


