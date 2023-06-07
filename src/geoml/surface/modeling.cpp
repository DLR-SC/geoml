#include "surface/modeling.h"

#include "geometry/curve-networks/InterpolateCurveNetwork.h"
#include "geometry/CurvesToSurface.h"

Handle(Geom_BSplineSurface)
geoml::interpolate_curve_network(const std::vector<Handle (Geom_Curve)> &ucurves,
                                 const std::vector<Handle (Geom_Curve)> &vcurves,
                                 double tolerance)
{
    geoml::InterpolateCurveNetwork interpolator(ucurves, vcurves, tolerance);

    return interpolator.Surface();
}

Handle(Geom_BSplineSurface)
geoml::interpolate_curves(const std::vector<Handle (Geom_Curve)> &ucurves, unsigned int max_degree,
                          bool join_continously)
{
    geoml::CurvesToSurface c2s(ucurves, join_continously);
    c2s.SetMaxDegree(max_degree);
    return c2s.Surface();
}
