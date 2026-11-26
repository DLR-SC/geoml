#include "Shape.h"
#include "topology/ShapeMetadata.h"


#include <TopExp_Explorer.hxx>

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace geoml {

Shape::Data::Data(TopoDS_Shape const& theShape,
                  std::shared_ptr<TopoMetaMap> const& topo_meta_map)
    : shape(theShape)
    , topo_meta_map(topo_meta_map)
{
    auto found = topo_meta_map->metadata_by_topology.find(shape);
    if (found == topo_meta_map->metadata_by_topology.end()) {
        metadata = std::make_shared<metadata::Metadata>();
        topo_meta_map->metadata_by_topology.emplace(shape, metadata);
    }
    else {
        metadata = found->second;
    }

    for (TopoDS_Iterator it(shape); it.More(); it.Next()) {
        children.emplace_back(Shape(it.Value(), topo_meta_map));
    }
}

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
    : Shape(theShape, std::make_shared<TopoMetaMap>())
{}

Shape::Shape()
    : Shape(TopoDS_Shape(), std::make_shared<TopoMetaMap>())
{}

Shape::Shape(TopoDS_Shape const& theShape,
             std::shared_ptr<TopoMetaMap> const& topo_meta_map)
    : m_data(std::make_shared<Data>(theShape, topo_meta_map))
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
    return details::is_descendent_of(*this, other.shape(), max_depth);
}

bool details::is_descendent_of(Shape const& shape, TopoDS_Shape const& other, int max_depth)
{
    FindIfVisitor visitor([&](Shape const& candidate) { return candidate.is_same(other); },
                          max_depth);
    shape.accept_history_visitor(visitor);
    return visitor.found;
}

bool Shape::is_ancestor_of(Shape const& other, int max_depth) const
{
    return other.is_descendent_of(*this);
}

bool Shape::has_tag(std::string const& tag) const
{
    auto const& key = metadata::keys::tags();
    auto const& data = metadata();
    if (!data.contains(key)) {
        return false;
    }
    auto const& tags = data.at(key);
    return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

void Shape::add_meta_tag(std::string const& tag)
{
    auto const& key = metadata::keys::tags();
    auto tags = metadata().find(key).value_or(std::vector<std::string>{});
    tags.push_back(tag);
    metadata().set(key, std::move(tags));
}

metadata::Metadata& Shape::metadata() noexcept
{
    return *m_data->metadata;
}

metadata::Metadata const& Shape::metadata() const noexcept
{
    return *m_data->metadata;
}

void Shape::detach_from_inputs(std::vector<Shape> const& inputs)
{
    // Selections may contain children from another model. Compare the actual
    // storage domains, since identical OCCT topology can have independent stores.
    std::unordered_set<TopoMetaMap const*> input_maps;
    auto collect_maps = [&](auto&& self, Shape const& node) -> void {
        input_maps.insert(node.m_data->topo_meta_map.get());
        for (auto const& child : node) {
            self(self, child);
        }
    };
    for (auto const& input : inputs) {
        collect_maps(collect_maps, input);
    }
    auto shares_input_storage = [&](Shape const& node) {
        return input_maps.count(node.m_data->topo_meta_map.get()) != 0;
    };
    if (!has_subshape_that(shares_input_storage)) {
        return;
    }

    // Copy wrapper data before history mapping. History parents and OCCT
    // geometry stay shared. Every repeated subshape in a model gets one store.
    std::unordered_map<TopoMetaMap const*, std::shared_ptr<TopoMetaMap>> copies;
    auto detach = [&](auto&& self, Shape& node) -> void {
        auto old = node.m_data;
        auto& model = copies[old->topo_meta_map.get()];
        if (!model) {
            model = std::make_shared<TopoMetaMap>();
        }
        auto [entry, inserted] = model->metadata_by_topology.try_emplace(old->shape);
        if (inserted) {
            entry->second = std::make_shared<metadata::Metadata>(*old->metadata);
        }
        node.m_data = std::make_shared<Data>(*old);
        node.m_data->topo_meta_map = model;
        node.m_data->metadata = entry->second;
        for (auto& child : node) {
            self(self, child);
        }
    };
    detach(detach, *this);
}

} // namespace geoml
