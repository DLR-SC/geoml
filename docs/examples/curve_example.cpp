// Minimal example: interpolate points to a B-spline curve
#include <vector>
#include <gp_Pnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include "geoml/curves/curves.h"

int main()
{
    gp_Pnt pt1(0.0, 0.0, 0.0);
    gp_Pnt pt2(1.0, 0.0, 0.0);
    gp_Pnt pt3(2.0, 0.0, 1.0);

    std::vector<gp_Pnt> input_points {pt1, pt2, pt3};

    // create B-spline curve interpolating the points
    Handle(Geom_BSplineCurve) curve = geoml::interpolate_points_to_b_spline_curve(input_points);

    // Use curve->StartPoint() / curve->EndPoint() etc.

    return 0;
}
