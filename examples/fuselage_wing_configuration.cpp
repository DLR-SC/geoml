#include <iostream>
#include <cmath>
#include <vector>

#include <geoml/curves/modeling.h>

#include <gp_Pnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include "Geom_Curve.hxx"
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

int main(){

/////////////////////////////////////////////////////
//// interplate NACA points to a B-spline curve: ////
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

return 0;
}


