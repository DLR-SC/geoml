#include "geoml/utility/utility.h"
#include "geoml/data_structures/conversions.h"


namespace geoml{

Array2d<gp_Pnt> extract_control_points_surface(const Handle(Geom_BSplineSurface) &b_spline_surface)
{
    return TCol_to_Array2d(b_spline_surface->Poles());
}


std::vector<gp_Pnt> extract_control_point_vector_in_U_direction (const Handle(Geom_BSplineSurface)& b_spline_surface, int j)
{
    Array2d<gp_Pnt> cp_net = extract_control_points_surface(b_spline_surface);

    return cp_net.getCol(j);
}


} // namespace geoml
