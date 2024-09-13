#include "Shape.hpp"

#include <TopExp_Explorer.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

#include <algorithm>

namespace geoml {

Shape::Shape(TopoDS_Shape const& theShape)
    : m_shape(theShape)
{
    std::vector<TopAbs_ShapeEnum> shape_types = {TopAbs_VERTEX, TopAbs_EDGE, TopAbs_FACE, TopAbs_SOLID};
    // add all subshapes. For now this is a big list with subshapes of all types.
    for (auto shape_type : shape_types) {
        for (TopExp_Explorer explorer(m_shape, shape_type); explorer.More(); explorer.Next())
        {
            if (!is_same(explorer.Current())) {
                m_subshapes.push_back(std::make_shared<Shape>(explorer.Current()));
            }
        }
    }
}

Shape::operator TopoDS_Shape() const {
    return m_shape;
}

std::vector<std::shared_ptr<Shape>> const& Shape::get_subshapes() const
{
    return m_subshapes;
}

std::vector<std::shared_ptr<Shape>>& Shape::get_subshapes()
{
    return m_subshapes;
}

bool Shape::is_type(TopAbs_ShapeEnum shape_type) const
{
    return m_shape.ShapeType() == shape_type;
}

bool Shape::is_same(Shape const& other) const
{
    return is_same(other.m_shape);
}

//IMPORTANT! This is the predicate used to check if two shapes match. Not sure if it is the best choice.
bool Shape::is_same(TopoDS_Shape const& other) const
{
    return m_shape.IsSame(other);
}

bool Shape::has_subshape(Shape const& shape) const
{
    return std::find_if(
               m_subshapes.begin(),
               m_subshapes.end(),
               [&](auto const& s){ return s->is_same(shape); }
               ) != m_subshapes.end();
}

bool Shape::has_origin() const
{
    return m_origins.size() > 0;
}

bool Shape::is_unmodified_descendent_of(Shape const& other, int max_depth) const
{
    return is_same(other) && is_descendent_of(other);
}

bool Shape::is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth) const
{
    for (auto const& other_subshape : other.get_subshapes()) {
        if (is_unmodified_descendent_of(*other_subshape, max_depth)) {
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
        if (is_modified_descendent_of(*other_subshape, max_depth)) {
            return true;
        }
    }
    return false;
}

bool Shape::is_descendent_of_subshape_in(Shape const& other, int max_depth) const
{
    for (auto const& other_subshape : other.get_subshapes()) {
        if (is_descendent_of(*other_subshape, max_depth)) {
            return true;
        }
    }
    return false;
}

bool Shape::is_child_of_subshape_in(Shape const& other) const
{
    return is_descendent_of_subshape_in(other, 1);
}

bool Shape::is_descendent_of(Shape const& other, int max_depth) const
{
    FindIfVisitor v(
        [&](Shape const& shape){ return shape.is_same(other); },
        max_depth
        );
    accept(v);
    return v.found;
}

bool Shape::is_child_of(Shape const& other) const
{
    return is_descendent_of(other, 1);
}

bool Shape::has_subshape_that_is_child_of(Shape const &shape) const
{
    return std::find_if(
           m_subshapes.begin(),
           m_subshapes.end(),
           [&](auto const& s){ return s->is_child_of(shape); }
           ) != m_subshapes.end();
}

bool Shape::has_tag(std::string const& tag)
{
    return std::find_if(
           m_persistent_meta_tags.begin(),
           m_persistent_meta_tags.end(),
           [&](auto const& s){ return s == tag; }
           ) != m_persistent_meta_tags.end(); 
}

std::vector<TagTrack>& Shape::get_tag_tracks()
{
    return m_tag_tracks;
}

const std::vector<TagTrack>& Shape::get_tag_tracks() const
{
    return m_tag_tracks;
}

void Shape::add_meta_tag(std::string const& tag) 
{
    m_persistent_meta_tags.push_back(tag);
}

void Shape::add_tag_track(TagTrack const& tt)
{
    m_tag_tracks.push_back(tt);
}

void Shape::apply_tag_tracks()
{
    for(auto &subshape : m_subshapes)
    {
        for(auto const& tag_track : m_tag_tracks)
        {
            if(tag_track.m_remainingSteps > 0 && tag_track.m_criterion(*subshape))
            {
                subshape->add_meta_tag(tag_track.m_tag);
            }
        }
    }
}


Shape create_cylinder(double radius, double height) {
    return Shape(BRepPrimAPI_MakeCylinder(radius, height));
}

Shape create_box(double dx, double dy, double dz) {
    return Shape(BRepPrimAPI_MakeBox(dx,dy,dz));
}


} // namespace geoml
