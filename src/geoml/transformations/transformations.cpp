#include "geoml/transformations/transformations.h"

#include "geometry/Transformation.h"


namespace geoml{

TopoDS_Shape translate(TopoDS_Shape const& origin, gp_Vec const& direction, double factor)
{
    auto t = Transformation(direction*factor);
    return t.Transform(origin);
}

} // namespace geoml
