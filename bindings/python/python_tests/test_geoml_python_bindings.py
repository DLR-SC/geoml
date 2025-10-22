from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt
from OCC.Display.SimpleGui import init_display 
from OCC.Core.Geom import Geom_BSplineCurve
from OCC.Core.Geom import Geom_Curve

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


def testing_nurbs_curve():
    
    p_1 = gp_Pnt(0.0, 0.0, 0.0)
    p_2 = gp_Pnt(0.0, 1.0, 2.0)
    p_3 = gp_Pnt(0.0, 2.0, -1.0)
    p_4 = gp_Pnt(1.0, 0.0, 3.0)
    p_5 = gp_Pnt(0.0, 4.0, 0.0)

    control_points = [p_1, p_2, p_3, p_4, p_5]

    weights = [1.0, 2.0, 1.0, 1.0, 1.0]

    knots = [0.0, 1.0, 2.0, 3.0]

    mults = [3, 1, 1, 3]

    degree = 2

    curve = pygeoml.nurbs_curve(containers.point_vector(control_points), 
                                containers.standard_real_vector(weights), 
                                containers.standard_real_vector(knots), 
                                containers.int_vector(mults), degree)

    return curve


def test_nurbs_curve():

    func_name = "nurbs_curve"
    func = getattr(pygeoml, func_name, None)

    print(f"Testing function: {func_name}")

    assert func is not None, f"{func_name} does not exist"

    try:
        result = testing_nurbs_curve()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


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
##################### test geoml/curves/BlendCurve.h ##########################
###########################################################################


def testing_blend_curve():

    p_11 = gp_Pnt(0.0,0.0,0.0)
    p_12 = gp_Pnt(1.0,0.0,0.0)       
    p_13 = gp_Pnt(2.0,0.0,0.0)

    point_list_1 = [p_11, p_12, p_13]

    curve_1 = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list_1), 2)
    edge_1 = pygeoml.CurveToEdge(curve_1)    
    enum_1 = pygeoml.GContinuity_G2

    start_connection = pygeoml.BlendCurveConnection(edge_1, p_13, enum_1)

    p_21 = gp_Pnt(0.0,0.0,0.0)
    p_22 = gp_Pnt(0.0,1.0,0.0)       
    p_23 = gp_Pnt(0.0,2.0,0.0)

    point_list_2 = [p_21, p_22, p_23]

    curve_2 = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list_2), 2)
    edge_2 = pygeoml.CurveToEdge(curve_2)    
    enum_2 = pygeoml.GContinuity_G2

    end_connection = pygeoml.BlendCurveConnection(edge_2, p_23, enum_2)

    blend_curve = pygeoml.BlendCurve(start_connection, end_connection)

    return blend_curve.blend_curve()


def test_blend_curve():

    try:
        result = testing_blend_curve()
    except Exception as e:
        pytest.fail(f"Calling {'testing_blend_curve'} raised an exception: {e}")


###########################################################################
##################### test geoml/surfaces/surfaces.h ######################
###########################################################################

def testing_curve_network_interpolation():

    # create u-curves
    p_11 = gp_Pnt(0.0,0.0,0.0)
    p_12 = gp_Pnt(0.0,1.0,0.0)       
    p_13 = gp_Pnt(1.0,2.0,0.0)
    point_list_u_1 = [p_11, p_12, p_13]
    curve_u_1 = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list_u_1), 2)

    p_21 = gp_Pnt(0.0,0.0,3.0)
    p_22 = gp_Pnt(0.0,1.0,3.0)       
    p_23 = gp_Pnt(1.0,2.0,3.0)
    point_list_u_2 = [p_21, p_22, p_23]    
    curve_u_2 = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list_u_2), 2)

    p_31 = gp_Pnt(0.0,4.0,3.0)
    p_32 = gp_Pnt(0.0,5.0,3.0)       
    p_33 = gp_Pnt(1.0,6.0,3.0)
    point_list_u_3 = [p_31, p_32, p_33]
    curve_u_3 = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list_u_3), 2)

    curve_list_u = [curve_u_1, curve_u_2, curve_u_3]

    # create v-curves
    point_list_v_1 = [p_11, p_21, p_31]
    curve_v_1 = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list_v_1), 2)

    point_list_v_2 = [p_12, p_22, p_32]
    curve_v_2 = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list_v_2), 2)

    point_list_v_3 = [p_13, p_23, p_33]
    curve_v_3 = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list_v_3), 2)
    curve_list_v = [curve_v_1, curve_v_2, curve_v_3]

    u_curves = containers.geomcurve_vector(curve_list_u)
    v_curves = containers.geomcurve_vector(curve_list_v)

    gordon_surface = pygeoml.interpolate_curve_network(u_curves, v_curves, 1e-1)

    return gordon_surface


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
#display_result(testing_interpolate_points_to_b_spline_curve())
# display_result(testing_create_face())
# display_result(testing_create_surface())

#display_result(testing_nurbs_curve())
display_result(testing_blend_curve())
display_result(testing_curve_network_interpolation())