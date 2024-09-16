#include "geoml/utility/utility.h"
#include "geoml/data_structures/conversions.h"


namespace geoml{

Array2d<gp_Pnt> extract_control_points_surface(const Handle(Geom_BSplineSurface) &b_spline_surface)
{
    return TCol_to_Array2d(b_spline_surface->Poles());
}

bool check_if_two_points_equal(gp_Pnt const& point_1, gp_Pnt const& point_2)
{
   return point_1.IsEqual(point_2, 1e-5);
}

Handle(Geom_Curve) flip_curve(Handle(Geom_Curve) const& curve, bool flip_me)
{
    return curve->Reversed();
}

} // namespace geoml
