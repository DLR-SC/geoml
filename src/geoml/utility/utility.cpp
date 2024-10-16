#include "geoml/utility/utility.h"
#include "geoml/data_structures/conversions.h"


namespace geoml{

Array2d<gp_Pnt> extract_control_points_surface(const Handle(Geom_BSplineSurface) &b_spline_surface)
{
    return TCol_to_Array2d(b_spline_surface->Poles());
}


std::vector<gp_Pnt> extract_control_point_vector_in_U_direction (const Handle(Geom_BSplineSurface)& b_spline_surface, int j)
{
    std::vector<gp_Pnt> vec;
    vec.reserve(b_spline_surface->NbUPoles());

    for(int i = 0; i < b_spline_surface->NbUPoles(); ++i)
    {
        vec.push_back(b_spline_surface->Poles().Value(i + 1, j + 1));
    }

    return vec;
}

std::vector<gp_Pnt> extract_control_point_vector_in_V_direction (const Handle(Geom_BSplineSurface)& b_spline_surface, int i)
{
    std::vector<gp_Pnt> vec;
    vec.reserve(b_spline_surface->NbVPoles());

    for(int j = 0; j < b_spline_surface->NbVPoles(); ++j)
    {
        vec.push_back(b_spline_surface->Poles().Value(i + 1, j + 1));
    }

    return vec;    

}


} // namespace geoml
