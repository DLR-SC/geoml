#include "geoml/transformations/transformations.h"
#include "geometry/Transformation.h"

#include <BRep_Builder.hxx>


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

TopoDS_Compound repeat_shape(TopoDS_Shape const& origin, Transform const& trans, int n_repeats)
{
    TopoDS_Compound result;
    BRep_Builder a_builder;

    TopoDS_Shape shape = origin;
    a_builder.MakeCompound(result);
    a_builder.Add(result, shape);
    for (int i=1; i < n_repeats; ++i) {
        shape = trans.operator()(shape);
        a_builder.Add(result, shape);
    }

    return result;
}

} // namespace geoml
