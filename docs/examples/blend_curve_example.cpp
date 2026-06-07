// Minimal example: create a transition curve between two edges
#include <vector>

#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>

#include "geoml/Continuity.h"
#include "geoml/curves/BlendCurve.h"
#include "geoml/curves/curves.h"
#include "geoml/geom_topo_conversions/geom_topo_conversions.h"

int main()
{
    std::vector<gp_Pnt> start_points{
        gp_Pnt(0.0, 0.0, 0.0),
        gp_Pnt(1.0, 0.0, 0.0),
        gp_Pnt(2.0, 1.0, 0.0)
    };

    std::vector<gp_Pnt> end_points{
        gp_Pnt(4.0, 3.0, 0.0),
        gp_Pnt(5.0, 4.0, 0.0),
        gp_Pnt(6.0, 4.0, 0.0)
    };

    Handle(Geom_Curve) start_curve =
        geoml::interpolate_points_to_b_spline_curve(start_points, 2);
    Handle(Geom_Curve) end_curve =
        geoml::interpolate_points_to_b_spline_curve(end_points, 2);

    TopoDS_Edge start_edge = geoml::CurveToEdge(start_curve);
    TopoDS_Edge end_edge = geoml::CurveToEdge(end_curve);

    geoml::BlendCurveConnection start_connection(
        start_edge, start_points.back(), geoml::GContinuity::G1);
    geoml::BlendCurveConnection end_connection(
        end_edge, end_points.front(), geoml::GContinuity::G1);

    TopoDS_Edge blended_edge = geoml::blend_curve(start_connection, end_connection);
    Handle(Geom_Curve) blended_curve = geoml::EdgeToCurve(blended_edge);

    // Use blended_curve->Value(parameter) to sample the transition curve.

    return 0;
}
