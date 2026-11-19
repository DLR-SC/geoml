// Minimal example: interpolate points to a B-spline curve and wrap it as a Shape
#include <vector>
#include <gp_Pnt.hxx>
#include <Geom_BSplineCurve.hxx>
#include <TopoDS_Edge.hxx>
#include "geoml/curves/curves.h"
#include "geoml/geom_topo_conversions/geom_topo_conversions.h"
#include "geoml/naming_choosing/Shape.h"

int main()
{
    gp_Pnt pt1(0.0, 0.0, 0.0);
    gp_Pnt pt2(1.0, 0.0, 0.0);
    gp_Pnt pt3(2.0, 0.0, 1.0);

    std::vector<gp_Pnt> input_points {pt1, pt2, pt3};

    // create B-spline curve interpolating the points
    Handle(Geom_BSplineCurve) curve = geoml::interpolate_points_to_b_spline_curve(input_points);

    // Use curve->StartPoint() / curve->EndPoint() etc.

    // convert the curve to an edge, wrap it in a geoml::Shape and name it with a meta tag
    TopoDS_Edge edge = geoml::CurveToEdge(curve);
    geoml::Shape shape(edge);
    shape.add_meta_tag("guide-curve");

    return 0;
}
