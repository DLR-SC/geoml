#include "geoml/utilities/utilities.h"
#include "geoml/data_structures/conversions.h"

#include <BRepFilletAPI_MakeFillet.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include "topology/BRepBuilderAPI_MakeShape_Operation.hpp"

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

Shape make_fillet (Shape const& solid , Shape const& edges, Standard_Real radius)
{
    BRepFilletAPI_MakeFillet MF (solid);
 
    auto operation = BRepBuilderAPI_MakeShape_Operation(&MF, {solid, edges});

    for (int i = 0; i< edges.size(); i++)
    {
        if (!edges[i].is_type(TopAbs_EDGE))
        {
            throw geoml::Error("A direct subshape of the second argument is not a TopoDS_EDGE", GENERIC_ERROR);
        }
        
        TopoDS_Edge temp_edge = TopoDS::Edge(edges[i]);
        MF.Add(radius, temp_edge);  
    }

    return operation.value();
}

} // namespace geoml
