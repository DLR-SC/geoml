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

// this wrapper class is here to make swig happy and because we dont understand how to wrap std::function directly.
class ShapePredicate
{
public:
    inline ShapePredicate(std::function<bool(Shape const&)> const& f) : fun(f) {}

    inline bool operator()(Shape const& s) const {
        return fun(s);
    }

    inline operator std::function<bool(Shape const&)>() {
        return fun;
    }

private:
    std::function<bool(Shape const&)> fun;
};

/**
* @brief A TagTrack stores
*  - a tag name
*  - the number of remaining modeling steps determining its lifetime in the modelling history 
*  - a logical criterion (predicate)
*/
struct TagTrack 
{
    template<typename Pred>
    GEOML_API_EXPORT TagTrack(std::string const& t, Pred&& criterion, int remainingSteps) 
        : m_tag(t), m_criterion(std::forward<Pred>(criterion)), m_remainingSteps(remainingSteps) {}

    GEOML_API_EXPORT TagTrack(std::string const& t, ShapePredicate const& criterion, int remainingSteps) 
        : m_tag(t), m_criterion(criterion), m_remainingSteps(remainingSteps) {}

    // the default ctr is for using std::vector<TagTrack> for the SWIG bindings (see pygeoml.i)    
    GEOML_API_EXPORT TagTrack()
        : m_criterion([](Shape const& ){return true;}) {} 

    ShapePredicate m_criterion;
    std::string m_tag;
    int m_remainingSteps;
};

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
GEOML_API_EXPORT std::size_t operator()(Shape const& s) const;
};

/**
 * @brief This class is used in the definition of ShapeContainer, 
 * It is a functor that determines, if two functions are to be considered
 * as equal.
 *
 * Currently, they are considered as equal, if TopoDS_Shape::IsSame returns
 * true.
 * 
 */
struct ShapeIsSame {
GEOML_API_EXPORT bool operator()(Shape const& l, Shape const& r) const;
};

using ShapeContainer = std::unordered_set<Shape, ShapeHasher, ShapeIsSame>;

} // namespace details


/**
 * @brief The Shape class is a container for the direct topological children 
 * as well as the direct modeling parents of a geometric entity.
 *
 * Internally, it wraps a TopoDS_Shape. It provides some interface
 * functions to explore the topological graph as well has the modeling
 * history graph. The topological graph is determined through the topological
 * information encoded in the wrapped TopoDS_Shape. The historical graph
 * is determined by functors derived from the CRTP class template geoml::Operation.
 * 
 * Shape is a light-weight wrapper around shared heap allocated memory.
 */
class Shape
{
    template <typename Derived>
    friend class Operation;

public:

    /**
     * @brief constructs an empty shape.
     *
     * It has a null TopoDS_Shape and no children
     */
    GEOML_API_EXPORT Shape();

    /**
     * @brief constructs a Shape from an existing TopoDS_Shape
     *
     * It extracts the children from the TopoDS_Shape and 
     * has no associated modeling history
     * 
     * @param theShape The OpenCascade shape
     */
    GEOML_API_EXPORT Shape(TopoDS_Shape const& theShape);

    /**
     * @brief converts a Shape to the wrapped TopoDS_Shape
     * 
     * @return TopoDS_Shape the wrapped OpenCascade shape
     */
    GEOML_API_EXPORT operator TopoDS_Shape() const;
    
    /**
     * @brief returns the wrapped TopoDS_Shape
     * 
     * @return TopoDS_Shape the wrapped OpenCascade shape
     */
    GEOML_API_EXPORT TopoDS_Shape shape() const;

    /**
     * @brief returns the direct topological subshapes of the shape
     * 
     * @return std::vector<Shape> the direct topological subshapes of the shape
     */
    GEOML_API_EXPORT std::vector<Shape> const& direct_subshapes() const;

    /**
     * @brief returns the direct topological subshapes of the shape
     * 
     * @return std::vector<Shape> the direct topological subshapes of the shape
     */
    GEOML_API_EXPORT std::vector<Shape>& direct_subshapes();

    /**
     * @brief returns all subshapes of the shape. 
     *
     * This includes the shape itself as well recursively all subshapes
     * of this shape's subshapes.
     * 
     * @return Shape a collection of all subshapes 
     */
    GEOML_API_EXPORT Shape get_subshapes() const;


    /**
     * @brief select_subshapes returns all subshapes, that satisfy a 
     * certain predicate. This function recurses to the subshape's
     * subshapes. The deepest level of recursion can be influenced via 
     * the max_depth argument.
     * 
     * @tparam Pred The predicate function taking a const referende to a Shape
     *              and returning a Boolean
     * @param f The predicate function
     * @param max_depth The maximum depth of recursion to search for subshapes
     *                  satisfying the given predicate. Defaults to the maximum
     *                  integer
     * @return Shape A Shape wrapping a topological unconnected TopoDS_Compound of subshapes
     *               satisfying the given predicate
     */
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
        Shape result{compound};

        // In the Shape constructor, new Shape instances are added as children
        // from the TopoDS_Shape instances stored in the compound. These do not 
        // yet have any historical data associated to them. To retain the historical
        // modeling connections, we have to overwrite the children vector with the 
        // result shapes from the visitor.
        result.m_data->children = std::vector<Shape>(v.results.begin(), v.results.end());

        return result;
    }

    /**
     * @brief select_subshapes returns all direct topological children, 
     * that satisfy a certain predicate. In contrast to select_subshapes, 
     * this function does not recurse to the subshape's subshapes.
     * 
     * @tparam Pred The predicate function taking a const reference to a Shape
     *              and returning a Boolean
     * @param f The predicate function
     * @return Shape A Shape wrapping a topological unconnected TopoDS_Compound of subshapes
     *               satisfying the given predicate
     */
    template <typename Pred>
    Shape filter(Pred&& f) const
    {
        auto pred = [=](Shape const& s) { return !s.is_same(*this) && f(s); };
        return select_subshapes(pred, 1);
    }

    /**
     * @brief returns the single topological child if it is unique. If there 
     * the shape has zero or more than one children, it raises an exception. 
     *
     * This function is useful to unwrap a single shape from a one-element 
     * compound/collection
     * 
     * @return the child shape 
     */
    GEOML_API_EXPORT Shape unique_element() const;

    /**
     * @brief returns the single topological child if it is unique. If there 
     * the shape has zero or more than one children, it returns the shape 
     * passed as argument. 
     *
     * This function is useful to unwrap a single shape from a one-element 
     * compound/collection. It allows a fallback instead of raising an exception.
     * 
     * @return the child shape 
     */
    GEOML_API_EXPORT Shape unique_element_or(Shape const& other) const;

    /**
     * @brief adds a tag to all subshapes (including the shape itself) that satisfy
     * a given predicate
     * 
     * @tparam Pred a function mapping a const reference of a Shape to a Boolean
     * @param f the predicate function
     * @param input_tag The tag that shall be applied to all subshapes satisfying the predicate
     */
    template <typename Pred>
    void add_meta_tag_to_subshapes(Pred&& f, std::string const& input_tag)
    {    
        Shape selection = this->select_subshapes(f);
        for (auto& selected_shape : selection)
        {
            selected_shape.add_meta_tag(input_tag);
        }
    }

    /**
     * @brief adds a string metadata (i.e. tag) to this shape
     * 
     * @param tag a string metadata tag
     */
    GEOML_API_EXPORT void add_meta_tag(std::string const& tag); // a meta_tag gets added to this shape (not to its subshapes)

    /**
     * @brief adds a TagTrack to this shape. A tagtrack is useful to 
     * preserve metadata tags even after geometry modifications. See
     * the documentation of TagTrack for more details.
     * 
     * @param tt 
     */
    GEOML_API_EXPORT void add_tag_track(TagTrack const& tt);

    /**
     * @brief returns all tag tracks associated with this shape
     * 
     * @return std::vector<TagTrack>&
     */
    GEOML_API_EXPORT std::vector<TagTrack>& get_tag_tracks();

    /**
     * @brief returns all tag tracks associated with this shape
     * 
     * @return std::vector<TagTrack> const&
     */
    GEOML_API_EXPORT const std::vector<TagTrack>& get_tag_tracks() const;

    /**
     * @brief applies the tag tracks associated to this shape.
     */
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
     * @param v the visitor
     * @param depth the current depth of the visitor in the graph
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
     * topology of a shape in DFS manner. It expects the
     * visitor to implement
     *
     *    bool Visitor::visit(Shape const& s, int depth)
     *    int Visitor::max_depth() const
     *
     *
     * that returns true, if the exploration is to be stopped
     * and false otherwise. The depth is passed in from accept
     *
     * @param v the visitor
     * @param depth the current depth of the visitor in the graph
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
    
    /**
     * @brief returns an iterator for the vector of direct topological children.
     */
    GEOML_API_EXPORT const_iterator begin() const;

    /**
     * @brief returns an iterator for the vector of direct topological children.
     */
    GEOML_API_EXPORT iterator begin();
    
    /**
     * @brief returns the past-the-end iterator for the vector of direct topological children. the 
     */
    GEOML_API_EXPORT const_iterator end() const;
    
    /**
     * @brief returns the past-the-end iterator for the vector of direct topological children. the 
     */
    GEOML_API_EXPORT iterator const end();
    
    /**
     * @brief retrieve the i-th direct topological child - (indexation starting from 0)
     * 
     * @param i the index of the topological child
     */
    GEOML_API_EXPORT Shape const& operator[](int i) const;
    
    /**
     * @brief retrieve the i-th direct topological child - (indexation starting from 0)
     * 
     * @param i the index of the topological child
     */
    GEOML_API_EXPORT Shape& operator[](int i);
    
    /**
     * @brief returns the number of direct topological children
     */
    GEOML_API_EXPORT size_t size() const;

    // The following are convenience functions that can be used
    // as predicates when selecting subshapes

    /**
     * @brief returns true if there are no direct topological children.
     * Returns false otherwise.
     *
     * The term "empty" here refers to the container of children, 
     * For example, a compound is empty if it does not contain any 
     * shapes. But for consistency, a vertex is also considered 
     * empty, as it doesn't have any direct topological children.
     *
     */
    GEOML_API_EXPORT bool is_empty() const;

    /**
     * @brief returns true if the wrapped TopoDS_Shape is null.
     * Returns false otherwise.
     */
    GEOML_API_EXPORT bool is_null() const;
    
    /**
     * @brief returns true, if the type of the wrapped TopoDS_Shape is the 
     * type passed as argument. Returns false otherwise.
     * 
     * @param shape_type the type of the shape
     */
    GEOML_API_EXPORT bool is_type(TopAbs_ShapeEnum shape_type) const;

    /**
     * @brief returns true, if this shape is the same as the other. 
     * This is true, if the wrapped shapes A and B satisfy A.IsSame(B).
     * 
     * @param other the other shape
     */
    GEOML_API_EXPORT bool is_same(Shape const& other) const;
    
    /**
     * @brief returns true, if this shape is the same as the other. 
     * This is true, if the wrapped shape A satisfies A.IsSame(other).
     * 
     * @param other the other shape
     */
    GEOML_API_EXPORT bool is_same(TopoDS_Shape const& other) const; //IMPORTANT! This is the predicate used to check if two shapes match. Not sure if it is the best choice.
    
    /**
     * @brief returns true, if this shape has a subshape s for which
     * s.is_same(shape) is true
     * 
     * @param shape the shape to check against
     * @return GEOML_API_EXPORT 
     */
    GEOML_API_EXPORT bool has_subshape(Shape const& shape) const;
    
    /**
     * @brief returns true, if it has an associated origin shape in the 
     * history graph
     */
    GEOML_API_EXPORT bool has_origin() const;

    /**
     * @brief returns true, if this shape is a historical descendent of 
     * the other shape, i.e. a result of modeling operations and it hasn't
     * been modified by any of the operations. The optional parameter max_depth
     * is the maximum depth to search in the history graph.
     * 
     * @param other the other shape
     * @param max_depth the maximum search depth. defaults to the maximum integer
     */
    GEOML_API_EXPORT bool is_unmodified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    
    /**
     * @brief returns true, if this shape is a historical descendent of a subshape in
     * the other shape, i.e. a result of modeling operations and it hasn't
     * been modified by any of the operations. The optional parameter max_depth
     * is the maximum depth to search in the history graph.
     * 
     * @param other the other shape
     * @param max_depth the maximum search depth. defaults to the maximum integer
     */
    GEOML_API_EXPORT bool is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    
    /**
     * @brief returns true, if this shape is a historical descendent of 
     * the other shape, i.e. a result of modeling operations and it has
     * been modified by any of the operations. The optional parameter max_depth
     * is the maximum depth to search in the history graph.
     * 
     * @param other the other shape
     * @param max_depth the maximum search depth. defaults to the maximum integer
     */
    GEOML_API_EXPORT bool is_modified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    
    /**
     * @brief returns true, if this shape is a historical descendent of a subshape in
     * the other shape, i.e. a result of modeling operations and it has
     * been modified by any of the operations. The optional parameter max_depth
     * is the maximum depth to search in the history graph.
     * 
     * @param other the other shape
     * @param max_depth the maximum search depth. defaults to the maximum integer
     */
    GEOML_API_EXPORT bool is_modified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    
    /**
     * @brief returns true, if this shape is a historical descendent of a subshape in
     * the other shape, i.e. a result of modeling operations. 
     * The optional parameter max_depth is the maximum depth to search in the history graph.
     * 
     * @param other the other shape
     * @param max_depth the maximum search depth. defaults to the maximum integer
     */
    GEOML_API_EXPORT bool is_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    
    
    /**
     * @brief returns true, if this shape is a historical descendent of 
     * the other shape, i.e. a result of modeling operations. 
     * The optional parameter max_depth is the maximum depth to search in the history graph.
     * 
     * @param other the other shape
     * @param max_depth the maximum search depth. defaults to the maximum integer
     */
    GEOML_API_EXPORT bool is_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    
    /**
     * @brief returns true, if this shape is a historical ancestor of
     * the other shape, i.e. the other shape results from this via modeling operations.
     * * The optional parameter max_depth is the maximum depth to search in the history graph.
     * 
     * @param other the other shape
     * @param max_depth the maximum search depth. defaults to the maximum integer
     */
    GEOML_API_EXPORT bool is_ancestor_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    
    /**
     * @brief returns true, if this shape has the given tag
     * 
     * @param tag a tag is a string metadata
     */
    GEOML_API_EXPORT bool has_tag(std::string const& tag) const;

    /**
     * @brief returns true if this shape has a subshape that 
     * satisfies the given predicate. This includes the shape itself
     * 
     * @tparam Pred A function mapping a const reference to a Shape to a Boolean
     * @param pred the predicate to check
     * @return true if the predicate is true for any of the subhshapes
     * @return false otherwise
     */
    template <typename Pred>
    bool has_subshape_that(Pred&& pred) const {
        auto v = details::FindIfVisitor<Pred>(std::forward<Pred>(pred));
        accept_topology_visitor(v);
        return v.found;
    }

private:

    /**
     * @brief The data of a Shape is stored in a shared_ptr to make sure
     * it is a lightweight wrapper around shared memory
     */
    struct Data {

        inline explicit Data(TopoDS_Shape const& theShape)
        : shape(theShape)
        {
            for (TopoDS_Iterator it(shape); it.More(); it.Next()) {
                    children.push_back(Shape(it.Value()));
            }
        }

        TopoDS_Shape shape; /** the wrapped TopoDS_Shape */

        std::vector<Shape> children; /** direct topology children */
        std::vector<Shape> origins;  /** direct history parents */

        std::vector<std::string> persistent_meta_tags; /** all persistent metatags */
        std::vector<TagTrack> tag_tracks; /** the associated tag tracks of a shape */
    };

    std::shared_ptr<Data> m_data; /** The data associated with this shape */

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

    /**
     * @brief checks the predicate for the current shape
     * 
     * @param shape the current shape in the graph
     * @param depth the current search depth
     * @return true, if the predicate is true for this or a previously visited shape
     * @return false, otherwise
     */
    bool visit(Shape const& shape, int depth) {
        if (depth <= m_max_depth) {
            found = m_f(shape);
        }
        return found;
    }

    /**
     * @brief returns the maximum depth for this visitor
     * 
     * @return int the maximum depth
     */
    int max_depth() const {
        return m_max_depth;
    }

    bool found; /** a flag, wether we have found a shape satisfying the predicate */

private:
    Pred m_f; /** the predicate function */
    int m_max_depth; /** the maximun search depth */
};

/**
 * @brief The FindVisitor class is a convenience visitor to
 * be used with Shape::accept_xxx_visitor to find shapes in the 
 * history graph or topology graph. It retains a set of all shapes
 * satisfying the predicate. Note that this set has unique elements,
 * in the sense that there can never be two elements a and b satsifying
 * a.is_same(b).
 */
template <typename Pred>
class FindVisitor
{
public:
    FindVisitor(Pred&& f, int max_depth = std::numeric_limits<int>::max())
        : m_f(std::forward<Pred>(f))
        , m_max_depth(max_depth)
    {}

    /**
     * @brief checks the predicate for the current shape and adds it to the 
     * set of shapes satsifying the given predicate, if we haven't exceeded the 
     * maximum search depth yet.
     * 
     * @param shape the current shape in the graph
     * @param depth the current search depth
     * @return true never
     * @return false always
     */
    bool visit(Shape const& shape, int depth) {
        if (depth <= max_depth() && m_f(shape)) {
            results.insert(shape);
        }
        return false;
    }

    /**
     * @brief the maximum depth for this visitor
     * 
     * @return int the maximum depth
     */
    int max_depth() const {
        return m_max_depth;
    }

    ShapeContainer results; /** the retained shapes satisfying the predicates */

private:
    Pred m_f; /** the predicate function */
    int m_max_depth; /** the maximum depth */
};

} // namespace details

/**
 * @brief This method adds a persistent meta tag to all subshapes of the input shape
 * satisfying the given predicate. Note that this includes the shape itself.
 * 
 * @tparam Pred the predicate function mapping a const reference to a Shape to a Boolean
 * @param input the input shape
 * @param f the predicate function
 * @param tag the tag to be added to all subshapes satisfying the given predicate
 */
template <typename Pred>
void add_persistent_meta_tag_to_subshapes(const Shape &input, Pred&& f, const std::string &tag)
{
    auto selection = input.select_subshapes(f);
    for (auto& selected_shape : selection)
    {
        selected_shape.add_meta_tag(tag);
    }
}

} // namespace geoml
