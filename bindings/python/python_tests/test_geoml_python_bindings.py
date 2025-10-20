from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt
from OCC.Display.SimpleGui import init_display 
from OCC.Core.Geom import Geom_BSplineCurve

import pytest


def display_result(shape):
    display, start_display, add_menu, add_function_to_menu = init_display()
    display.DisplayShape(shape)
    display.FitAll()
    start_display()


def bspline_curve(cp, knots, mults, degree):
    occ_cp = containers.point_array(cp)
    occ_knots = containers.float_array(knots)
    occ_mults = containers.int_array(mults)
    curve = Geom_BSplineCurve(occ_cp.Array1(), occ_knots.Array1(), occ_mults.Array1(), degree)

    return curve


###########################################################################
###########################################################################


###########################################################################
##################### test geoml/occ_helpers/containers.py ################
###########################################################################


def testing_point_vector():
    p_1 = gp_Pnt(0.0,0.0,0.0)
    p_2 = gp_Pnt(0.5,0.0,0.0)       
    p_3 = gp_Pnt(1.0,1.0,0.0)

    point_list = [p_1, p_2, p_3]

    return containers.point_vector(point_list)

def test_point_vector():

    func_name = "point_vector"
    func = getattr(containers, func_name, None)

    print(f"Testing function: {func_name}")

    assert func is not None, f"{func_name} does not exist"

    try:
        result = testing_point_vector()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


###########################################################################
##################### test geoml/curves/curves.h ##########################
###########################################################################


def testing_interpolate_points_to_b_spline_curve():
     
    p_1 = gp_Pnt(0,0,0)
    p_2 = gp_Pnt(0.5,0,0)       
    p_3 = gp_Pnt(1,1,0)

    point_list = [p_1, p_2, p_3]

    curve = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list), 2)
    
    return curve


def test_interpolate_points_to_b_spline_curve():

    func_name = "interpolate_points_to_b_spline_curve"
    func = getattr(pygeoml, func_name, None)

    print(f"Testing function: {func_name}")

    assert func is not None, f"{func_name} does not exist"

    try:
        result = testing_interpolate_points_to_b_spline_curve()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


###########################################################################
##################### test geoml/surfaces/surfaces.h ######################
###########################################################################


def testing_create_surface():
    x = gp_Pnt(0,0,0)
    y = gp_Pnt(0.5,0,0)       
    z = gp_Pnt(1,1,0)
    w = gp_Pnt(0,3,0)
    
    return pygeoml.create_surface(x,y,z,w)


def test_create_surface():
    func_name = "create_surface"
    func = getattr(pygeoml, func_name, None)

    print(f"Testing function: {func_name}")

    assert func is not None, f"{func_name} does not exist"

    try:
        result = testing_create_surface()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


def testing_create_face():
    x = gp_Pnt(0,0,0)
    y = gp_Pnt(0.5,0,0)       
    z = gp_Pnt(1,1,0)
    w = gp_Pnt(0,3,0)
    
    return pygeoml.create_face(x,y,z,w)


def test_create_face():
    func_name = "create_face"
    func = getattr(pygeoml, func_name, None)

    print(f"Testing function: {func_name}")

    assert func is not None, f"{func_name} does not exist"

    try:
        result = testing_create_face()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


###########################################################################
###########################################################################

#display_result(testing_point_vector())
display_result(testing_interpolate_points_to_b_spline_curve())
# display_result(testing_create_face())
# display_result(testing_create_surface())
