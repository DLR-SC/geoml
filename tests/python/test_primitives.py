from geoml import pygeoml

import pytest 


###########################################################################
##################### test geoml/primitives/modeling.hpp ################## 
###########################################################################


def test_primitives():
    # test: Shape create_cylinder(Standard_Real radius, Standard_Real height);

    radius = 2.0
    height = 8.0

    cylinder_shape = pygeoml.create_cylinder(radius, height)
    assert type(cylinder_shape) is pygeoml.Shape


    # test: Shape create_box(Standard_Real dx, Standard_Real dy, Standard_Real dz);

    dx = 1.0
    dy = 2.0
    dz = 4.0

    box_shape = pygeoml.create_box(dx, dy, dz)
    assert type(box_shape) is pygeoml.Shape

