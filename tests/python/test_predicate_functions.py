from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt
from OCC.Core.TopAbs import TopAbs_EDGE, TopAbs_FACE

import pytest 


###########################################################################
##################### test geoml\predicates\predicate_functions.h #########
###########################################################################


def compute_Shape_constructor():
    p_1 = gp_Pnt(0.0,0.0,2.0)
    p_2 = gp_Pnt(1.0,5.0,0.0)       
    p_3 = gp_Pnt(2.0,0.0,0.0)

    point_list = [p_1, p_2, p_3]

    curve = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list), 2)
    edge = pygeoml.CurveToEdge(curve)

    return pygeoml.Shape(edge)

def test_predicate_functions_h():

    edge_shape = compute_Shape_constructor()
    edge_shape.add_meta_tag("added_tag")

    # test: ShapePredicate operator&&(ShapePredicate const& l, ShapePredicate const& r);
    # CAUTION: pred_1 or pred_2 doesn't give an error message!
    pred_1 = pygeoml.has_tag("test_string_1")
    assert pred_1(edge_shape) == False

    pred_2 = pygeoml.has_tag("test_string_2")
    assert pred_2(edge_shape) == False

    pred_1_and_2 = pred_1 & pred_2
    assert pred_1_and_2(edge_shape) == False

    edge_shape.add_meta_tag("test_string_1")
    edge_shape.add_meta_tag("test_string_2")

    assert pred_1_and_2(edge_shape) == True

    # test: ShapePredicate operator||(ShapePredicate const& l, ShapePredicate const& r);
    # CAUTION: pred_1 and pred_2 doesn't give an error message! 
    pred_3 = pygeoml.has_tag("test_string_3")

    pred_1_or_3 = pred_1 | pred_3
    assert pred_1_or_3(edge_shape) == True

    pred_3_or_3 = pred_3 | pred_3
    assert pred_3_or_3(edge_shape) == False

    # test: ShapePredicate operator!(ShapePredicate const& pred);
    # CAUTION:  not pred doesn't give an error message!
    pred_1_not = ~pred_1
    assert pred_1_not(edge_shape) == False

    # test: ShapePredicate has_tag(std::string const& tag);
    pred = pygeoml.has_tag("added_tag")
    assert pred(edge_shape) == True

    # test: ShapePredicate is_type(TopAbs_ShapeEnum shape_type);
    pred = pygeoml.is_type(TopAbs_EDGE)
    assert pred(edge_shape) == True

    pred = pygeoml.is_type(TopAbs_FACE)
    assert pred(edge_shape) == False

    # test: ShapePredicate is_same(Shape const& other);
    pred = pygeoml.is_same(edge_shape)
    assert pred(edge_shape) == True

    edge_shape_1 = compute_Shape_constructor()
    assert pred(edge_shape_1) == False

    # test: ShapePredicate is_same(TopoDS_Shape const& other); 
    p_1 = gp_Pnt(0.0,0.0,0.0)
    p_2 = gp_Pnt(1.0,0.0,0.0)       
    p_3 = gp_Pnt(2.0,0.0,0.0)

    point_list = [p_1, p_2, p_3]

    curve = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list), 2)
    edge = pygeoml.CurveToEdge(curve)

    pred = pygeoml.is_same(edge)
    assert pred(edge_shape) == False 

    # test: ShapePredicate has_subshape(Shape const& shape);
    pred = pygeoml.has_subshape(edge_shape)
    assert pred(edge_shape) == True

    # test: ShapePredicate is_subshape_of(Shape const& other);
    pred = pygeoml.is_subshape_of(edge_shape)
    assert pred(edge_shape) == True

    # test: ShapePredicate is_unmodified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max());
    pred = pygeoml.is_unmodified_descendent_of(edge_shape)
    assert pred(edge_shape) == True

    # test: ShapePredicate is_unmodified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max());
    pred = pygeoml.is_unmodified_descendent_of_subshape_in(edge_shape)
    assert pred(edge_shape) == True

    # test: ShapePredicate is_modified_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max());
    pred = pygeoml.is_modified_descendent_of(edge_shape)
    assert pred(edge_shape) == False

    # test: ShapePredicate is_modified_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max());
    pred = pygeoml.is_modified_descendent_of_subshape_in(edge_shape)
    assert pred(edge_shape) == False

    # test: ShapePredicate is_descendent_of_subshape_in(Shape const& other, int max_depth=std::numeric_limits<int>::max());
    pred = pygeoml.is_descendent_of_subshape_in(edge_shape)
    assert pred(edge_shape) == True

    # test: ShapePredicate is_descendent_of(Shape const& other, int max_depth=std::numeric_limits<int>::max());
    pred = pygeoml.is_descendent_of(edge_shape)
    assert pred(edge_shape) == True

    # test: ShapePredicate has_subshape_that(ShapePredicate const& pred);
    pred_is_edge = pygeoml.is_type(TopAbs_EDGE)
    pred = pygeoml.has_subshape_that(pred_is_edge)
    assert pred(edge_shape) == True

