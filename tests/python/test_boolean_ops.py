from geoml import pygeoml
from OCC.Core.BRepPrimAPI import BRepPrimAPI_MakeBox, BRepPrimAPI_MakeSphere

import pytest 


###########################################################################
##################### test geoml/boolean_ops/modeling.hpp #################
###########################################################################


def test_boolean_ops():

    box = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape()
    sphere = BRepPrimAPI_MakeSphere(5.0).Shape()

    box_shape = pygeoml.Shape(box)
    sphere_shape = pygeoml.Shape(sphere)

    # test: Shape boolean_subtract(Shape const& shape, Shape const& cutting_tool);
    subtracted_shape = pygeoml.boolean_subtract(box_shape, sphere_shape)
    assert type(subtracted_shape) is pygeoml.Shape

    # test: Shape operator-(Shape const& shape, Shape const& cutting_tool);
    subtracted_shape_1 = sphere_shape - box_shape
    assert type(subtracted_shape) is pygeoml.Shape

    # test: Shape boolean_union(Shape const& shape_1, Shape const& shape_2);
    united_shape = pygeoml.boolean_union(box_shape, sphere_shape)
    assert type(subtracted_shape) is pygeoml.Shape

    # test: Shape operator+(Shape const& shape_1, Shape const& shape_2);
    united_shape_1 = box_shape + sphere_shape
    assert type(united_shape_1) is pygeoml.Shape

    