#pragma once

#include "geoml/geoml.h"

#include <vector>
#include <memory>
#include <string>
#include <functional>

#include <TopoDS_Shape.hxx>

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
    GEOML_API_EXPORT Shape(TopoDS_Shape const& theShape);

    // conversion to TopoDS_Shape
    GEOML_API_EXPORT operator TopoDS_Shape() const;

    GEOML_API_EXPORT std::vector<std::shared_ptr<Shape>> const& get_subshapes() const;

    GEOML_API_EXPORT std::vector<std::shared_ptr<Shape>>& get_subshapes();

    template <typename Pred>
    std::vector<std::shared_ptr<Shape>> select_subshapes(Pred&& f)
    {
        std::vector<std::shared_ptr<Shape>> ret;
        for (auto const& subshape : m_subshapes)
        {
            if (f(*subshape)) {
                ret.push_back(subshape);
            }
        }
        return ret;
    }

    template <typename Pred>
    void add_meta_tag_to_subshapes(Pred&& f, std::string const& input_tag) // this function can only add a tag to a subshape of the shape and not the shape itself
    {    
        std::vector<std::shared_ptr<Shape>> selection = this->select_subshapes(f);

        for (auto const& selected_shape : selection)
        {
            selected_shape->add_meta_tag(input_tag);
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
     *
     *
     * that returns true, if the exploration is to be stopped
     * and false otherwise. The depth is passed in from accept
     *
     * @param v
     */
    template <typename Visitor>
    bool accept(Visitor&& v, int depth=0) const
    {
        bool stop = v.visit(*this, depth);
        if (!stop) {
            for(auto const& origin : m_origins) {
                if(origin->accept(v, depth+1)) {
                    return true;
                }
            }
        }
        return stop;
    }

    // The following are convenience functions that can be used
    // as predicates when selecting subshapes

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
    GEOML_API_EXPORT bool is_child_of_subshape_in(Shape const& other) const;
    GEOML_API_EXPORT bool is_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    GEOML_API_EXPORT bool is_child_of(Shape const& other) const;
    GEOML_API_EXPORT bool has_subshape_that_is_child_of(Shape const &shape) const;
    GEOML_API_EXPORT bool has_tag(std::string const& tag) const;

private:

         /**
         * @brief The FindIfVisitor class is a convenience visitor to
         * be used with Shape::accept to find shapes in the history graph
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
            if (depth > m_max_depth) {
                return true;
            }
            found = m_f(shape);
            return m_f(shape);
        }

        bool found;

    private:
        Pred m_f;
        int m_max_depth;
    };

    TopoDS_Shape m_shape;

    std::vector<std::shared_ptr<Shape>> m_subshapes;
    std::vector<std::shared_ptr<Shape>> m_origins;  // history parents

    std::vector<std::string> m_persistent_meta_tags;
    std::vector<TagTrack> m_tag_tracks; 

};

template <typename Pred>
void add_persistent_meta_tag_to_subshapes(const Shape &input, Pred&& f, const std::string &tag)
{
    std::vector<std::shared_ptr<Shape>> selection = input.select_subshapes(f);
    for (auto const& selected_shape : selection)
    {
        selected_shape->add_meta_tag(tag);
    }
}

} // namespace geoml
