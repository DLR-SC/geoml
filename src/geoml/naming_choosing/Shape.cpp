#include "Shape.h"


#include <TopExp_Explorer.hxx>

#include <algorithm>
#include <string>

namespace geoml {

namespace details {

std::size_t ShapeHasher::operator()(Shape const& s) const
{
    // Use the address of TShape and the hash of the Location
    TopoDS_Shape occt_shape = s;
    const void* tShapePtr = occt_shape.TShape().get();
    std::size_t tShapeHash = std::hash<const void*>{}(tShapePtr);
    std::size_t locationHash = occt_shape.Location().HashCode(std::numeric_limits<int>::max());
    return tShapeHash ^ (locationHash << 1); // Combine with a left shift
}

bool ShapeIsSame::operator()(Shape const& l, Shape const& r) const
{
    return l.is_same(r);
}

} // namespace details 


Shape::Shape(TopoDS_Shape const& theShape)
    : m_data(std::make_shared<Data>(theShape))
{}

Shape::Shape()
    : m_data(std::make_shared<Data>(TopoDS_Shape()))
{}

Shape::operator TopoDS_Shape() const {
    return m_data->shape;
}

TopoDS_Shape Shape::shape() const {
    return m_data->shape;
}

std::vector<Shape> const& Shape::direct_subshapes() const
{
    return m_data->children;
}

Shape::const_iterator Shape::begin() const 
{
    return m_data->children.begin();
}

Shape::iterator Shape::begin() 
{
    return m_data->children.begin();
}

Shape::const_iterator Shape::end() const
{
    return m_data->children.end();
}

Shape::iterator const Shape::end()
{
    return m_data->children.end();
}


Shape const& Shape::operator[](int i) const 
{
    return m_data->children[i];
}

Shape& Shape::operator[](int i) 
{
    return m_data->children[i];
}

size_t Shape::size() const
{
    if (m_data->shape.IsNull()) {
        return 0;
    }
    if (m_data->shape.ShapeType() == TopAbs_COMPOUND) {
        return m_data->children.size();
    }
    return 1;
}

bool Shape::is_null() const
{
    return m_data->shape.IsNull();
}

bool Shape::is_empty() const
{
    return size() == 0;
}

std::vector<Shape>& Shape::direct_subshapes()
{
    return m_data->children;
}

Shape Shape::get_subshapes() const
{
    return select_subshapes([](auto&&){ return true; });
}

Shape Shape::unique_element() const 
{
    if (m_data->shape.ShapeType() != TopAbs_COMPOUND) {
        throw Error("unique_element: The shape is not a compound.");
    }
    if (m_data->children.size() != 1) {
        throw Error(std::string("unique_element only works for shapes with exactly one child. This shape has ") + std::to_string(m_data->children.size()) + " children.");
    }

    return m_data->children[0];
}

Shape Shape::unique_element_or(Shape const& other) const 
{
    if (m_data->shape.ShapeType() == TopAbs_COMPOUND && m_data->children.size() == 1) {
        return m_data->children[0];
    }
    return other;
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
    return has_subshape_that([=](Shape const& s) { return s.is_same(shape); });
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
    details::FindIfVisitor v(
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


} // namespace geoml
