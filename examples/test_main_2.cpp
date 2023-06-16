
#include <iostream>
#include <cmath>
#include <vector>
#include <initializer_list>

#include "geolib/BSplineCurve.hpp"
#include "geolib/CurvesToSurface.hpp"
#include "geolib/PointsToBSplineInterpolation.hpp"

#include "geolib/WritingMethods.hpp"
#include "geolib/BSplineAlgorithms.hpp" 

#include "Transformation.hpp"

#include "filletEdge.hpp"

#include <gp_Pnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include "BRepPrimAPI_MakeBox.hxx"
#include "BRepGProp.hxx"
#include "GProp_GProps.hxx"
#include <GeomAPI_Interpolate.hxx>
#include "Geom_Curve.hxx"
#include "Geom_Surface.hxx"
#include <Geom_BSplineSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomFill_BSplineCurves.hxx>
#include <GeomFill_Coons.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Shape.hxx> 
#include <TopoDS.hxx> 
#include <TopoDS_Solid.hxx> 

#include <Geom_TrimmedCurve.hxx>

#include <gp_GTrsf.hxx>

#include <gp_Mat.hxx>

#include <gp_XYZ.hxx>


using namespace geo;

int main(){

////
//// interplate NACA points to a B-spline curve:
////

// list of 21 points on NACA2412 profile:
std::vector<double> px {1.000084, 0.975825, 0.905287, 0.795069, 0.655665, 0.500588, 0.34468, 0.203313, 0.091996, 0.022051, 0.0, 0.026892, 0.098987, 0.208902, 0.346303, 0.499412, 0.653352, 0.792716, 0.90373, 0.975232, 0.999916};
std::vector<double> pz {0.001257, 0.006231, 0.019752, 0.03826, 0.057302, 0.072381, 0.079198, 0.072947, 0.054325, 0.028152, 0.0, -0.023408, -0.037507, -0.042346, -0.039941, -0.033493, -0.0245, -0.015499, -0.008033, -0.003035, -0.001257};

// create a curve in y=0 plane:
Handle(TColgp_HArray1OfPnt) points_NACA = new TColgp_HArray1OfPnt(1,21);

for(int i = 1; i<=px.size() ;++i)
{
	points_NACA->SetValue(i,gp_Pnt(px.at(i-1), pz.at(i-1) , 0.0));
}

// use TiGL's class to create interpolation curve: 
PointsToBSplineInterpolation interpolatorNACA (points_NACA,3,false);

Handle_Geom_Curve mySplineNACA_Geom_Curve_interpolator =interpolatorNACA.Curve();

// define transformations of profiles: 
gp_Trsf first_traf;
first_traf.SetValues(100.,0.,0.,0.,
					0., 100., 0., 0.,
					0., 0., 1., 0.);


Transformation my_first_trafo (first_traf);

gp_Trsf second_traf;
second_traf.SetValues(80.,0.,0.,0.,
					0., 100., 0., 0.,
					0., 0., 1., 50.);

Transformation my_second_trafo (second_traf);

gp_Trsf third_traf;
third_traf.SetValues(50.,0.,0.,0.,
					0., 100., 0., 20.,
					0., 0., 1., 100.);

Transformation my_third_trafo (third_traf);

// now create the three curves via the defined three transformations

Handle_Geom_Curve my_first_transformed_profile = transform(interpolatorNACA.Curve(), my_first_trafo);
Handle_Geom_Curve my_second_transformed_profile = transform(interpolatorNACA.Curve(), my_second_trafo);
Handle_Geom_Curve my_third_transformed_profile = transform(interpolatorNACA.Curve(), my_third_trafo);

// write
writeToStp(my_first_transformed_profile, "my_first_transformed_profile.stp");
writeToStp(my_second_transformed_profile, "my_second_transformed_profile.stp");
writeToStp(my_third_transformed_profile, "my_third_transformed_profile.stp");

// now create the wing surface from the transformed NACA profiles:
std::vector<Handle(Geom_Curve)> vec_geom_NACA_crvs {my_first_transformed_profile,
	my_second_transformed_profile, my_third_transformed_profile};

// create the surface via the free function interpolate_curves (in CurvesToSurface.hpp):
Handle(Geom_BSplineSurface) result_surf_wing = interpolate_curves(vec_geom_NACA_crvs);

writeGeomEntityToStepFile(result_surf_wing, "surface_wing.stp");

//// we reached cp:2 (cf. diagram in Miro)

////
//// 
////

return 0;
}


