#include "geoml/utility/utility.h"
#include "geoml/data_structures/conversions.h"


namespace geoml{

Array2d<gp_Pnt> extract_control_points_surface(const Handle(Geom_BSplineSurface) &b_spline_surface)
{
    std::cout << "here we are!3" << std::endl;
    TColgp_Array2OfPnt net = b_spline_surface->Poles();
    std::cout << "here we are!4" << std::endl;
    Array2d<gp_Pnt> net_array = TCol_to_Array2d(net);
    std::cout << "here we are!5" << std::endl;
    return net_array;
    // return TCol_to_Array2d(b_spline_surface->Poles());
}

} // namespace geoml
