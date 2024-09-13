#include <geoml/surfaces/modeling.h>
#include <geoml/internal/geometry/PointsToBSplineInterpolation.h>


#include <iostream>
#include <vector>

using namespace geoml;

int main(){

	// list of 21 points on NACA2412 profile:
	std::vector<double> px {1.000084, 0.975825, 0.905287, 0.795069, 0.655665, 0.500588, 0.34468, 0.203313, 0.091996, 0.022051, 0.0, 0.026892, 0.098987, 0.208902, 0.346303, 0.499412, 0.653352, 0.792716, 0.90373, 0.975232, 0.999916};
	std::vector<double> pz {0.001257, 0.006231, 0.019752, 0.03826, 0.057302, 0.072381, 0.079198, 0.072947, 0.054325, 0.028152, 0.0, -0.023408, -0.037507, -0.042346, -0.039941, -0.033493, -0.0245, -0.015499, -0.008033, -0.003035, -0.001257};

	// create a curve in y=o plane:

	Handle(TColgp_HArray1OfPnt) points_NACA = new TColgp_HArray1OfPnt(1,21);

	for(int i = 1; i<=px.size() ;++i)
	{
		points_NACA->SetValue(i,gp_Pnt(px.at(i-1), 0.0, pz.at(i-1)));
	}

	geoml::PointsToBSplineInterpolation points2bspline(points_NACA);
	Handle_Geom_Curve mySplineNACA_Geom_Curve =points2bspline.Curve();


	return 0;
}


