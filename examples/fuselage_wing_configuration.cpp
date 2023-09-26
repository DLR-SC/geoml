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

#include <BRepPrimAPI_MakeRevol.hxx>
#include <GC_MakeSegment.hxx>

#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"



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

/*
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
*/

void writeGeomEntityToStepFile(const TopoDS_Shape& my_shape, std::string fileName)
{
STEPControl_Writer writer;
writer.Transfer(my_shape,STEPControl_AsIs);
writer.Write(fileName.c_str());
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


///////////////////////////////////// break /////////////////////////////////////
///////////////////////////////////// break /////////////////////////////////////
///////////////////////////////////// break /////////////////////////////////////


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

Handle(Geom_BSplineCurve) my_Geom_Curve_1 
        = new Geom_BSplineCurve(control_points_1, weights_1, knots_1, mults_1, degree_1);

writeToStp(my_Geom_Curve_1, "test_curve_1.stp");


// now create a cylindrical surface:

// create a profile curve for the cylinder
gp_Pnt start_point(1, 0, 0);
gp_Pnt end_point(1, 0, 2); // The height of the cylinder

TColgp_Array1OfPnt profile_points(1,2);
profile_points.SetValue(1,start_point);
profile_points.SetValue(2,end_point);

// degree:
Standard_Integer degree = 1;

// weights:
TColStd_Array1OfReal weights(1, 2);
weights.SetValue(1, 1.0);  
weights.SetValue(2, 1.0);  

// knots:
TColStd_Array1OfReal knots(1,2); 
knots.SetValue(1,0.0);           
knots.SetValue(2,1.0);

// multiplicities: 
TColStd_Array1OfInteger mults(1,2); 
mults.SetValue(1,2);                
mults.SetValue(2,2);

// create the profile curve:
Handle(Geom_BSplineCurve) profile_curve 
        = new Geom_BSplineCurve(profile_points, weights, knots, mults, degree);

// convert the Geom_BSplineCurve in a TopoDS_Shape:
BRepBuilderAPI_MakeEdge edgeMaker;
edgeMaker.Init(profile_curve);
TopoDS_Shape profile_curve_shape = edgeMaker.Shape();

// create the cylinder by revolving the profile curve
BRepPrimAPI_MakeRevol revol_maker(profile_curve_shape, gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 3.1415); 

TopoDS_Shape cylinder = revol_maker.Shape();

std::cout << "hello world again" << std::endl;

writeGeomEntityToStepFile(cylinder, "my_cylinder_test.stp");

std::cout << "hello hello from the other side" << std::endl;

 // calculate the area of the surface
 //TopoDS_Face face = BRepBuilderAPI_MakeFace(result_surf, 1e-5);
 GProp_GProps properties;
 BRepGProp::SurfaceProperties(cylinder, properties);

 double area = properties.Mass();

std::cout << " the surface area is: " << area << std::endl;


return 0;
}


