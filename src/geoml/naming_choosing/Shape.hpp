#pragma once

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

struct TagTrack {

    TagTrack(std::string& t, std::function<bool(Shape const&)> &criterion, int remainingSteps) 
        : m_tag(t), m_criterion(criterion), m_remainingSteps(remainingSteps) {}

    std::function<bool(Shape const&)> m_criterion;
    std::string m_tag;
    int m_remainingSteps;
};

/**
 * @brief The Shape class is a wrapper around TopoDS_Shape, that stores
 *  - its subshapes
 *  - a reference to its origin in the parametric modelling history
 *  - some functionality to select subshapes
 */
class Shape
{
    template <typename Derived>
    friend class Operation;

public:
    Shape(TopoDS_Shape const& theShape);

    // conversion to TopoDS_Shape
    operator TopoDS_Shape() const;

    std::vector<std::shared_ptr<Shape>> const& get_subshapes() const;

    std::vector<std::shared_ptr<Shape>>& get_subshapes();

    void set_name(std::string &name);                          

    std::string get_name();

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
    void check_and_add_persistent_meta_tag_deep(Pred&& f, std::string input_tag) // tag added to this shape itself or/and to its subshapes (investigate morge if/when shape itself is included in m_subshapes)
    {
        std::vector<std::shared_ptr<Shape>> selection = this->select_subshapes(f);

        for (auto const& selected_shape : selection)
        {
            selected_shape->add_persistent_meta_tag_only_for_this_shape(input_tag);
        }
    }
                                                           
    void add_persistent_meta_tag_only_for_this_shape(std::string tag) // a meta_tag gets added to this shape (not to its subshapes)
    {
        persistent_meta_tags.push_back(tag);
    }

    void apply_track_tags()
    {
        for(auto &subshape : m_subshapes)
        {
            for(auto & tag_track : m_tag_tracks)
            {
                if(tag_track.m_remainingSteps > 0 && tag_track.m_criterion(*subshape))
                {
                    subshape->add_persistent_meta_tag_only_for_this_shape(tag_track.m_tag);
                }
            }
        }
    }

    auto const& get_metadata() const;

    /**
     * @brief accept accpets a Visitor that explores the
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

    bool is_type(TopAbs_ShapeEnum shape_type) const;
    bool is_same(Shape const& other) const;
    bool is_same(TopoDS_Shape const& other) const; //IMPORTANT! This is the predicate used to check if two shapes match. Not sure if it is the best choice.
    bool has_subshape(Shape const& shape) const;
    bool has_origin() const;
    bool is_unmodified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    bool is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    bool is_modified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    bool is_modified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    bool is_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    bool is_child_of_subshape_in(Shape const& other) const;
    bool is_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    bool is_child_of(Shape const& other) const;

    bool has_subshape_that_is_child_of(Shape const &shape) const;

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

    std::size_t m_id;
    std::vector<std::shared_ptr<Shape>> m_subshapes;
    std::vector<std::shared_ptr<Shape>> m_origins;  // history parents

    std::string m_name;
    std::vector<std::string> persistent_meta_tags;
    std::vector<TagTrack> m_tag_tracks; 
};

Shape create_cylinder(double radius, double height);

Shape create_box(double dx, double dy, double dz);


template <typename Pred>
void add_persistent_meta_tag_to_shape(const Shape &input, Pred&& f, const std::string &tag)
{
    std::vector<std::shared_ptr<Shape>> selection = input.select_subshapes(f);
    for (auto const& selected_shape : selection)
    {
        selected_shape->add_persistent_meta_tag_only_for_this_shape(tag);
    }
}

} // namespace geoml
