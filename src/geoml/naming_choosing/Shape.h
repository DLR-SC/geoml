#pragma once

#include "geoml/geoml.h"
#include "geoml/error.h"

#include <limits>
#include <stdexcept>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <unordered_set>

#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>

namespace geoml {

// forward declaration
namespace internal {
template <typename Derived>
class Operation;
}

class Shape;

/**
* @brief A TagTrack stores
*  - a tag name
*  - the number of remaining modeling steps determining its lifetime in the modelling history 
*  - a logical criterion
*/
struct TagTrack {

    GEOML_API_EXPORT TagTrack(std::string const& t, std::function<bool(Shape const&)> const& criterion, int remainingSteps) 
        : m_tag(t), m_criterion(criterion), m_remainingSteps(remainingSteps) {}

    std::function<bool(Shape const&)> m_criterion;
    std::string m_tag;
    int m_remainingSteps;
};

// forward declarations
class Shape;

namespace details {

template <typename Pred>
class FindVisitor;

template <typename Pred>
class FindIfVisitor;

/**
 * @brief The ShapeHasher returns a hash for a shape that is in line with 
 * TopoDS_Shape::IsSame. The latter returns true, if the two input shapes 
 * share the same underlying TopoDS_TShape and have the same location. 
 * Therefor ShapeHasher returns a combination of the TopoDS_TShape address
 * and the hash of the location. This way ShapeHasher(a) == ShapeHasher(b)
 * if and only if TopoDS_Shape(a).IsSame(b), modulo hash collisions
 * 
 */
struct ShapeHasher {
std::size_t operator()(Shape const& s) const;
};

struct ShapeIsSame {
bool operator()(Shape const& l, Shape const& r) const;
};

using ShapeContainer = std::unordered_set<Shape, ShapeHasher, ShapeIsSame>;

} // namespace details

/**
 * @brief The Shape class is a wrapper around TopoDS_Shape, that stores
 *  - its subshapes (the Shape itself is not included in its subshapes!)
 *  - a reference to its origin in the parametric modelling history
 *  - some functionality to select subshapes
 */
class Shape
{
    template <typename Derived>
    friend class Operation;

public:

    GEOML_API_EXPORT Shape();
    GEOML_API_EXPORT Shape(TopoDS_Shape const& theShape);

    // conversion to TopoDS_Shape
    GEOML_API_EXPORT operator TopoDS_Shape() const;

    GEOML_API_EXPORT std::vector<Shape> const& get_children() const;

    GEOML_API_EXPORT std::vector<Shape>& get_children();

    GEOML_API_EXPORT Shape get_subshapes() const;

    template <typename Pred>
    Shape select_subshapes(Pred&& f, int max_depth = std::numeric_limits<int>::max()) const
    {
        auto v = details::FindVisitor<Pred>(std::forward<Pred>(f), max_depth);
        accept_topology_visitor(v);

        TopoDS_Compound compound;
        BRep_Builder builder;
        builder.MakeCompound(compound);
        for (auto const& s : v.results) {
            builder.Add(compound, s);
        }
        Shape result = compound;

        // overwrite children to make sure, the data is retained
        result.m_data->children = std::vector<Shape>(v.results.begin(), v.results.end());

        return result;
    }

    template <typename Pred>
    Shape filter(Pred&& f) const
    {
        auto pred = [=](Shape const& s) { return !s.is_same(*this) && f(s); };
        return select_subshapes(pred, 1);
    }

    GEOML_API_EXPORT Shape unique_element() const;

    GEOML_API_EXPORT Shape unique_element_or(Shape const& other) const;

    template <typename Pred>
    void add_meta_tag_to_subshapes(Pred&& f, std::string const& input_tag) // this function can only add a tag to a subshape of the shape and not the shape itself
    {    
        Shape selection = this->select_subshapes(f);
        for (auto& selected_shape : selection)
        {
            selected_shape.add_meta_tag(input_tag);
        }
    }
                                                           
    GEOML_API_EXPORT void add_meta_tag(std::string const& tag); // a meta_tag gets added to this shape (not to its subshapes)

    GEOML_API_EXPORT void add_tag_track(TagTrack const& tt);

    GEOML_API_EXPORT std::vector<TagTrack>& get_tag_tracks();

    GEOML_API_EXPORT const std::vector<TagTrack>& get_tag_tracks() const;

    GEOML_API_EXPORT void apply_tag_tracks();

    /**
     * @brief accept accepts a Visitor that explores the
     * history of a shape in DFS manner. It expects the
     * visitor to implement
     *
     *    bool Visitor::visit(Shape const& s, int depth)
     *    int Visitor::max_depth() const
     *
     *
     * that returns true, if the exploration is to be stopped
     * and false otherwise. The depth is passed in from accept
     *
     * @param v
     */
    template <typename Visitor>
    bool accept_history_visitor(Visitor& v, int depth=0) const
    {
        bool stop = v.visit(*this, depth);
        if (!stop && depth < v.max_depth()) {
            for(auto const& origin : m_data->origins) {
                if(origin.accept_history_visitor(v, depth+1)) {
                    return true;
                }
            }
        }
        return stop;
    }

    /**
     * @brief accept accepts a Visitor that explores the
     * history of a shape in DFS manner. It expects the
     * visitor to implement
     *
     *    bool Visitor::visit(Shape const& s, int depth)
     *    int Visitor::max_depth() const
     *
     *
     * that returns true, if the exploration is to be stopped
     * and false otherwise. The depth is passed in from accept
     *
     * @param v
     */
    template <typename Visitor>
    bool accept_topology_visitor(Visitor& v, int depth=0) const
    {
        bool stop = v.visit(*this, depth);
        if (!stop && depth < v.max_depth()) {
            for(auto const& child : m_data->children) {
                if (child.accept_topology_visitor(v, depth+1)) {
                    return true;
                }
            }
        }
        return stop;
    }

    using iterator = std::vector<Shape>::iterator;
    using const_iterator = std::vector<Shape>::const_iterator;
    GEOML_API_EXPORT const_iterator begin() const;
    GEOML_API_EXPORT iterator begin();
    GEOML_API_EXPORT const_iterator end() const;
    GEOML_API_EXPORT iterator const end();
    GEOML_API_EXPORT Shape const& operator[](int i) const;
    GEOML_API_EXPORT Shape& operator[](int i);
    GEOML_API_EXPORT size_t size() const;

    // The following are convenience functions that can be used
    // as predicates when selecting subshapes

    GEOML_API_EXPORT bool is_empty() const;
    GEOML_API_EXPORT bool is_type(TopAbs_ShapeEnum shape_type) const;
    GEOML_API_EXPORT bool is_same(Shape const& other) const;
    GEOML_API_EXPORT bool is_same(TopoDS_Shape const& other) const; //IMPORTANT! This is the predicate used to check if two shapes match. Not sure if it is the best choice.
    GEOML_API_EXPORT bool has_subshape(Shape const& shape) const;
    GEOML_API_EXPORT bool has_origin() const;
    GEOML_API_EXPORT bool is_unmodified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    GEOML_API_EXPORT bool is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    GEOML_API_EXPORT bool is_modified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    GEOML_API_EXPORT bool is_modified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    GEOML_API_EXPORT bool is_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    GEOML_API_EXPORT bool is_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    GEOML_API_EXPORT bool is_ancestor_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    GEOML_API_EXPORT bool has_tag(std::string const& tag) const;

    template <typename Pred>
    bool has_subshape_that(Pred&& pred) const {
        auto v = details::FindIfVisitor<Pred>(std::forward<Pred>(pred));
        accept_topology_visitor(v);
        return v.found;
    }

private:

    // The data of a Shape is stored in a shared_ptr to make sure
    // it is a lightweight wrapper around shared memory
    struct Data {

        inline explicit Data(TopoDS_Shape const& theShape)
        : shape(theShape)
        {
            for (TopoDS_Iterator it(shape); it.More(); it.Next()) {
                    children.push_back(Shape(it.Value()));
            }
        }

        TopoDS_Shape shape;

        std::vector<Shape> children; // direct topology children
        std::vector<Shape> origins;  // direct history parents

        std::vector<std::string> persistent_meta_tags;
        std::vector<TagTrack> tag_tracks; 
    };

    std::shared_ptr<Data> m_data;

};

namespace details {

/**
 * @brief The FindIfVisitor class is a convenience visitor to
 * be used with Shape::accept_xxx_visitor to find shapes in the 
 * history graph or topology graph. It returns true if a shape 
 * is found matching the predicate
 */
template <typename Pred>
class FindIfVisitor
{
public:
    FindIfVisitor(Pred&& f, int max_depth = std::numeric_limits<int>::max())
        : found(false)
        , m_f(std::forward<Pred>(f))
        , m_max_depth(max_depth)
    {}

    bool visit(Shape const& shape, int depth) {
        if (depth <= m_max_depth) {
            found = m_f(shape);
        }
        return found;
    }

    int max_depth() const {
        return m_max_depth;
    }

    bool found;

private:
    Pred m_f;
    int m_max_depth;
};

/**
 * @brief The FindVisitor class is a convenience visitor to
 * be used with Shape::accept_xxx_visitor to find shapes in the 
 * history graph or topology graph. It returns a vector of all shapes
 * satisfying the predicate
 */
template <typename Pred>
class FindVisitor
{
public:
    FindVisitor(Pred&& f, int max_depth = std::numeric_limits<int>::max())
        : m_f(std::forward<Pred>(f))
        , m_max_depth(max_depth)
    {}

    bool visit(Shape const& shape, int depth) {
        if (depth <= max_depth() && m_f(shape)) {
            results.insert(shape);
        }
        return false;
    }

    int max_depth() const {
        return m_max_depth;
    }

    ShapeContainer results;

private:
    Pred m_f;
    int m_max_depth;
};

} // namespace details

template <typename Pred>
void add_persistent_meta_tag_to_subshapes(const Shape &input, Pred&& f, const std::string &tag)
{
    std::vector<Shape> selection = input.select_subshapes(f);
    for (auto& selected_shape : selection)
    {
        selected_shape.add_meta_tag(tag);
    }
}

} // namespace geoml
