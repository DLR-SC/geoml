#include "Shape.h"

#include "topology/MakeCompound_Operation.h"
#include <TopExp_Explorer.hxx>

#include <algorithm>

namespace geoml {

Shape::Shape(TopoDS_Shape const& theShape)
    : m_data(std::make_shared<Data>(theShape))
{}

Shape::Shape()
    : m_data(std::make_shared<Data>(TopoDS_Shape()))
{}

Shape::operator TopoDS_Shape() const {
    return m_data->shape;
}

std::vector<Shape> const& Shape::get_children() const
{
    return m_data->children;
}

GEOML_API_EXPORT bool Shape::is_empty() const
{
    return m_data->children.size() == 0;
}

std::vector<Shape>& Shape::get_children()
{
    return m_data->children;
}

GEOML_API_EXPORT std::vector<Shape> Shape::get_subshapes() const
{
    return select_subshapes([](auto&&){ return true; });
}

bool Shape::is_type(TopAbs_ShapeEnum shape_type) const
{
    return m_data->shape.ShapeType() == shape_type;
}

bool Shape::is_same(Shape const& other) const
{
    return is_same(other.m_data->shape);
}

//IMPORTANT! This is the predicate used to check if two shapes match. Not sure if it is the best choice.
bool Shape::is_same(TopoDS_Shape const& other) const
{
    return m_data->shape.IsSame(other);
}

bool Shape::has_subshape(Shape const& shape) const
{
    auto v = FindIfVisitor([=](Shape const& other){ return other.is_same(shape); });
    accept_topology_visitor(v);
    return v.found;
}

bool Shape::has_origin() const
{
    return m_data->origins.size() > 0;
}

bool Shape::is_unmodified_descendent_of(Shape const& other, int max_depth) const
{
    return is_same(other) && is_descendent_of(other);
}

bool Shape::is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth) const
{
    for (auto const& other_subshape : other.get_subshapes()) {
        if (is_unmodified_descendent_of(other_subshape, max_depth)) {
            return true;
        }
    }
    return false;
}

bool Shape::is_modified_descendent_of(Shape const& other, int max_depth) const
{
    return !is_same(other) && is_descendent_of(other);
}

bool Shape::is_modified_descendent_of_subshape_in(Shape const& other, int max_depth) const
{
    for (auto const& other_subshape : other.get_subshapes()) {
        if (is_modified_descendent_of(other_subshape, max_depth)) {
            return true;
        }
    }
    return false;
}

bool Shape::is_descendent_of_subshape_in(Shape const& other, int max_depth) const
{
    return other.has_subshape_that([=](Shape const& s){ return s.is_ancestor_of(*this); });
}

bool Shape::is_descendent_of(Shape const& other, int max_depth) const
{
    FindIfVisitor v(
        [&](Shape const& shape){ return shape.is_same(other); },
        max_depth
        );
    accept_history_visitor(v);
    return v.found;
}

bool Shape::is_ancestor_of(Shape const& other, int max_depth) const
{
    return other.is_descendent_of(*this);
}

bool Shape::has_tag(std::string const& tag) const
{
    return std::find_if(
           m_data->persistent_meta_tags.begin(),
           m_data->persistent_meta_tags.end(),
           [&](auto const& s){ return s == tag; }
           ) != m_data->persistent_meta_tags.end(); 
}

std::vector<TagTrack>& Shape::get_tag_tracks()
{
    return m_data->tag_tracks;
}

const std::vector<TagTrack>& Shape::get_tag_tracks() const
{
    return m_data->tag_tracks;
}

void Shape::add_meta_tag(std::string const& tag) 
{
    m_data->persistent_meta_tags.push_back(tag);
}

void Shape::add_tag_track(TagTrack const& tt)
{
    m_data->tag_tracks.push_back(tt);
}

void Shape::apply_tag_tracks()
{
    for(auto &subshape : get_subshapes()) //TODO: A for each would be cool
    {
        for(auto const& tag_track : m_data->tag_tracks)
        {
            if(tag_track.m_remainingSteps > 0 && tag_track.m_criterion(subshape))
            {
                subshape.add_meta_tag(tag_track.m_tag);
            }
        }
    }
}

Shape Shape::vector_of_shape_to_shape(std::vector<Shape> const& shapes) const
{
    if (shapes.size() == 0) {
            // return an empty shape
            return Shape();
    }

    if (shapes.size() == 1) {
        return shapes[0];
    }

    MakeCompound_Operation op(shapes);
    return op.value();
}


} // namespace geoml
