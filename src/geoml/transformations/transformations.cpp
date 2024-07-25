#include "geoml/transformations/transformations.h"

#include "geometry/Transformation.h"


namespace geoml{

TopoDS_Shape translate(TopoDS_Shape const& origin, gp_Vec const& direction, double factor)
{
    auto t = Transformation(direction*factor);
    
    return t.Transform(origin);
}

gp_Pnt translate(gp_Pnt const& origin, gp_Vec const& direction, double factor)
{
    auto t = Transformation(direction*factor);

    return t.Transform(origin);
}

std::vector<gp_Pnt> translate(std::vector<gp_Pnt> const& origin, gp_Vec const& direction, double factor)
{
    auto t = Transformation(direction*factor);

    std::vector<gp_Pnt> transformed_points;

    for(gp_Pnt p: origin){
        transformed_points.push_back(t.Transform(p));
    }

    return transformed_points;
}

} // namespace geoml
