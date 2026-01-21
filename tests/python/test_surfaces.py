from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt, gp_Vec
from OCC.Core.Geom import Geom_BSplineCurve, Geom_Curve, Geom_BSplineSurface, Geom_Surface
from OCC.Core.TopoDS import TopoDS_Shape, TopoDS_Edge, TopoDS_Face

import pytest 


###########################################################################
##################### test geoml/surfaces/surfaces.h ######################
###########################################################################


def test_curve_network_interpolation():

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

    result = pygeoml.interpolate_curve_network(u_curves, v_curves, 1e-1)
    assert type(result) is Geom_BSplineSurface


def test_interpolate_curves():

    p_11 = gp_Pnt(0.0,0.0,0.0)
    p_12 = gp_Pnt(0.0,1.0,0.0)       
    p_13 = gp_Pnt(1.0,2.0,0.0)
    point_list_1 = [p_11, p_12, p_13]
    curve_1 = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list_1), 2)

    p_21 = gp_Pnt(0.0,0.0,3.0)
    p_22 = gp_Pnt(0.0,1.0,3.0)       
    p_23 = gp_Pnt(1.0,2.0,3.0)
    point_list_2 = [p_21, p_22, p_23]    
    curve_2 = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list_2), 2)

    p_31 = gp_Pnt(0.0,4.0,3.0)
    p_32 = gp_Pnt(0.0,5.0,3.0)       
    p_33 = gp_Pnt(1.0,6.0,3.0)
    point_list_3 = [p_31, p_32, p_33]
    curve_3 = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list_3), 2)

    curve_list = [curve_1, curve_2, curve_3]

    result = pygeoml.interpolate_curves(containers.geomcurve_vector(curve_list), 2, True)
    assert type(result) is Geom_BSplineSurface


def test_revolving_shape():

    p_1 = gp_Pnt(2.0, 0.0, 0.0)
    p_2 = gp_Pnt(1.0, 0.0, 1.0)       
    p_3 = gp_Pnt(3.0, 0.0, 2.0)

    point_list = [p_1, p_2, p_3]

    curve = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list), 2)
    edge = pygeoml.CurveToEdge(curve)

    start_point_rotation_axis = gp_Pnt(0.0, 0.0, 0.0)

    rotation_axis_direction = gp_Vec(0.0, 0.0, 1.0)

    result = pygeoml.revolving_shape(edge, start_point_rotation_axis, rotation_axis_direction)
    assert type(result) is TopoDS_Face


def test_nurbs_surface():

    p_11 = gp_Pnt(0.0,0.0,0.0)
    p_12 = gp_Pnt(0.0,1.0,0.0)       
    p_13 = gp_Pnt(1.0,2.0,0.0)

    p_21 = gp_Pnt(0.0,0.0,3.0)
    p_22 = gp_Pnt(0.0,1.0,3.0)       
    p_23 = gp_Pnt(1.0,2.0,3.0)

    p_31 = gp_Pnt(0.0,4.0,3.0)
    p_32 = gp_Pnt(0.0,5.0,3.0)       
    p_33 = gp_Pnt(1.0,6.0,3.0)

    control_points = pygeoml.Array2dgp_Pnt(3, 3)

    control_points.setValue(0, 0, p_11)
    control_points.setValue(0, 1, p_12)
    control_points.setValue(0, 2, p_13)
    control_points.setValue(1, 0, p_21)
    control_points.setValue(1, 1, p_22)
    control_points.setValue(1, 2, p_23)
    control_points.setValue(2, 0, p_31)
    control_points.setValue(2, 1, p_32)
    control_points.setValue(2, 2, p_33)

    w_11 = 1.0 
    w_12 = 2.0       
    w_13 = 1.0

    w_21 = 1.0
    w_22 = 1.0      
    w_23 = 1.0

    w_31 = 1.0
    w_32 = 1.0
    w_33 = 1.0

    weights = pygeoml.Array2dStandardReal(3, 3)

    weights.setValue(0, 0, w_11)
    weights.setValue(0, 1, w_12)
    weights.setValue(0, 2, w_13)
    weights.setValue(1, 0, w_21)
    weights.setValue(1, 1, w_22)
    weights.setValue(1, 2, w_23)
    weights.setValue(2, 0, w_31)
    weights.setValue(2, 1, w_32)
    weights.setValue(2, 2, w_33)

    u_knots = [0.0, 1.0]
    v_knots = [0.0, 1.0]

    u_mults = [3, 3]
    v_mults = [3, 3]

    u_degree = 2
    v_degree = 2

    result = pygeoml.nurbs_surface(control_points, weights, 
                                containers.standard_real_vector(u_knots), 
                                containers.standard_real_vector(v_knots), 
                                containers.int_vector(u_mults),
                                containers.int_vector(v_mults),
                                u_degree, v_degree)

    assert type(result) is Geom_BSplineSurface


def test_create_surface():
    x = gp_Pnt(0,0,0)
    y = gp_Pnt(0.5,0,0)       
    z = gp_Pnt(1,1,0)
    w = gp_Pnt(0,3,0)

    result = pygeoml.create_surface(x,y,z,w)    
    assert type(result) is Geom_Surface


def test_create_face():
    x = gp_Pnt(0,0,0)
    y = gp_Pnt(0.5,0,0)       
    z = gp_Pnt(1,1,0)
    w = gp_Pnt(0,3,0)

    result = pygeoml.create_face(x,y,z,w)
    assert type(result) is TopoDS_Face

