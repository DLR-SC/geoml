from geoml import pygeoml
from geoml.pygeoml import is_vertex, is_edge
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt
from OCC.Core.TopoDS import TopoDS_Shape, TopoDS_Edge, TopoDS_Face, TopoDS_Solid
from OCC.Core.TopAbs import TopAbs_EDGE, TopAbs_FACE, TopAbs_VERTEX, TopAbs_SHAPE

import pytest 


###########################################################################
##################### test geoml\naming_choosing\Shape.h ##################
###########################################################################


def test_Shape_h():

    # create a TopoDS_EDGE
    p_1 = gp_Pnt(0.0,0.0,2.0)
    p_2 = gp_Pnt(1.0,5.0,0.0)       
    p_3 = gp_Pnt(2.0,0.0,0.0)

    point_list = [p_1, p_2, p_3]

    curve = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list), 2)
    edge = pygeoml.CurveToEdge(curve)

    # test: Shape(TopoDS_Shape const& theShape);
    edge_shape = pygeoml.Shape(edge)

    # test: TopoDS_Shape shape() const;
    wrapped_shape = edge_shape.shape()
    assert type(wrapped_shape) is TopoDS_Edge

    # wrapped_shapet: operator TopoDS_Shape() const;
    # -> does not work currently

    # test: std::vector<Shape>& direct_subshapes();
    # sub_shapes = edge_shape.direct_subshapes()
    edge_shape.direct_subshapes()
    assert type(edge_shape) is pygeoml.Shape

    # test: Shape get_subshapes() const;
    edge_shape.get_subshapes()
    assert type(edge_shape) is pygeoml.Shape

    # test: void add_meta_tag(std::string const& tag);
    edge_shape.add_meta_tag("added_tag")
    assert type(edge_shape) is pygeoml.Shape

    # test: template <typename Pred>
    # Shape select_subshapes(Pred&& f, int max_depth = std::numeric_limits<int>::max()) const
    pred_1 = pygeoml.has_tag("not_added_tag")
    pred_2 = pygeoml.has_tag("added_tag")

    edge_shape.select_subshapes(pred_1)
    assert type(edge_shape) is pygeoml.Shape

    edge_shape.select_subshapes(pred_2)
    assert type(edge_shape) is pygeoml.Shape

    # test: template <typename Pred>
    # Shape filter(Pred&& f) const
    edge_shape.filter(pred_2)
    assert type(edge_shape) is pygeoml.Shape

    # test: Shape unique_element() const;
    # edge_shape.unique_element() #get an error, probably as expected

    # test: Shape unique_element_or(Shape const& other) const;
    edge_shape.unique_element_or(edge_shape)
    assert type(edge_shape) is pygeoml.Shape

    # test: template <typename Pred>
    # void add_meta_tag_to_subshapes(Pred&& f, std::string const& input_tag)
    edge_shape.add_meta_tag_to_subshapes(pred_2, "another_added_tag")
    assert type(edge_shape) is pygeoml.Shape

    # test: void add_tag_track(TagTrack const& tt);
    tag_track = pygeoml.TagTrack("added_by_tag_track_if_is_edge", is_edge, 2)
    
    edge_shape.add_tag_track(tag_track)
    assert type(edge_shape) is pygeoml.Shape

    # test: std::vector<TagTrack>& get_tag_tracks();
    tag_tracks = edge_shape.get_tag_tracks()
    assert type(tag_tracks) is pygeoml.TagTrackList


    # test: void apply_tag_tracks();
    edge_shape.apply_tag_tracks()
    assert type(edge_shape) is pygeoml.Shape
    
    assert edge_shape.has_tag("not_added_tag") == False
    assert edge_shape.has_tag("added_tag") == True
    assert edge_shape.has_tag("another_added_tag") == True
    assert edge_shape.has_tag("added_by_tag_track_if_is_edge") == True

    # test: Shape operator[](int i);
    assert type(edge_shape[0]) is pygeoml.Shape 
    assert type(edge_shape[1]) is pygeoml.Shape 
    assert type(edge_shape[2]) is pygeoml.Shape 
    assert type(edge_shape[3]) is pygeoml.Shape 
    assert type(edge_shape[4]) is pygeoml.Shape 
    assert type(edge_shape[5]) is pygeoml.Shape 
    assert type(edge_shape[6]) is pygeoml.Shape 

    # test: size_t size() const;
    assert edge_shape.size() == 1

    # test: bool is_empty() const;
    assert edge_shape.is_empty() == False

    # test: bool is_null() const;
    assert edge_shape.is_null() == False

    # test: bool is_type(TopAbs_ShapeEnum shape_type) const;
    assert edge_shape.is_type(TopAbs_EDGE) == True

    # test: bool is_same(Shape const& other) const;
    edge_shape_1 = pygeoml.Shape(edge)

    assert edge_shape.is_same(edge_shape_1) == True
    assert edge_shape.is_same(edge_shape) == True

    # test: bool is_same(TopoDS_Shape const& other) const; 
    assert edge_shape.is_same(edge) == True

    # test: bool has_subshape(Shape const& shape) const;
    assert edge_shape.has_subshape(edge_shape) == True
    assert edge_shape.has_subshape(edge_shape_1) == True

    # test:  bool has_origin() const;
    assert edge_shape.has_origin() == False

    # test: bool bool is_unmodified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    assert edge_shape.is_unmodified_descendent_of(edge_shape) == True

    # test: bool is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    assert edge_shape.is_unmodified_descendent_of_subshape_in(edge_shape) == True

    # test: bool is_modified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    assert edge_shape.is_modified_descendent_of(edge_shape) == False

    # test: bool is_modified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    assert edge_shape.is_modified_descendent_of_subshape_in(edge_shape) == False

    # test: bool is_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    assert edge_shape.is_descendent_of_subshape_in(edge_shape) == True

    # test: bool is_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    assert edge_shape.is_descendent_of(edge_shape) == True

    # test: bool is_ancestor_of(Shape const& other, int max_depth=std::numeric_limits<int>::max()) const;
    assert edge_shape.is_ancestor_of(edge_shape) == True

    # test: bool has_tag(std::string const& tag) const;
    assert edge_shape.has_tag("tag_example") == False

    # test: template <typename Pred>
    # bool has_subshape_that(Pred&& pred) const;
    pred_is_edge = pygeoml.is_type(TopAbs_EDGE)
    pred = pygeoml.has_subshape_that(pred_is_edge)
    assert pred(edge_shape) == True

    # test (free function): template <typename Pred>
    # void add_persistent_meta_tag_to_subshapes(const Shape &input, Pred&& f, const std::string &tag) 
    pygeoml.add_persistent_meta_tag_to_subshapes(edge_shape, pred_is_edge, "added_via_free_function")
    assert edge_shape.has_tag("added_via_free_function") == True
    
