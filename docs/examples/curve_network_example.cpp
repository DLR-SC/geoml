// Minimal example: interpolate a surface through a network of crossing curves
#include <vector>

#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>

#include "geoml/curves/curves.h"
#include "geoml/surfaces/surfaces.h"

namespace
{
Handle(Geom_Curve) interpolate_curve(std::vector<gp_Pnt> const& points)
{
    return geoml::interpolate_points_to_b_spline_curve(points, 2);
}
}

int main()
{
    gp_Pnt p00(0.0, 0.0, 0.0);
    gp_Pnt p01(0.0, 1.0, 0.0);
    gp_Pnt p02(1.0, 2.0, 0.0);

    gp_Pnt p10(0.0, 0.0, 3.0);
    gp_Pnt p11(0.0, 1.0, 3.0);
    gp_Pnt p12(1.0, 2.0, 3.0);

    gp_Pnt p20(0.0, 4.0, 3.0);
    gp_Pnt p21(0.0, 5.0, 3.0);
    gp_Pnt p22(1.0, 6.0, 3.0);

    std::vector<Handle(Geom_Curve)> u_curves{
        interpolate_curve({p00, p01, p02}),
        interpolate_curve({p10, p11, p12}),
        interpolate_curve({p20, p21, p22})
    };

    std::vector<Handle(Geom_Curve)> v_curves{
        interpolate_curve({p00, p10, p20}),
        interpolate_curve({p01, p11, p21}),
        interpolate_curve({p02, p12, p22})
    };

    Handle(Geom_BSplineSurface) surface =
        geoml::interpolate_curve_network(u_curves, v_curves, 1e-1);

    // Use surface->D0(u, v, point) to evaluate points on the surface.

    return 0;
}
