from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.Geom import Geom_BSplineCurve, Geom_Curve
from OCC.Core.TopoDS import TopoDS_Edge
from OCC.Core.gp import gp_Pnt, gp_Vec

import pytest 


###########################################################################
##################### test geoml/curves/curves.h ##########################
###########################################################################


def compute_nurbs_curve():
    
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

    result = pygeoml.nurbs_curve(containers.point_vector(control_points), 
                            containers.standard_real_vector(weights), 
                            containers.standard_real_vector(knots), 
                            containers.int_vector(mults), degree)
    
    assert type(result) is Geom_BSplineCurve 


def test_interpolate_points_to_b_spline_curve():
     
    p_1 = gp_Pnt(0,0,0)
    p_2 = gp_Pnt(0.5,0,0)       
    p_3 = gp_Pnt(1,1,0)

    point_list = [p_1, p_2, p_3]

    result = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list), 2)
    assert type(result) is Geom_BSplineCurve


###########################################################################
##################### test geoml/curves/BlendCurve.h ##########################
###########################################################################


def test_blend_curve():
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

    result = blend_curve.blend_curve()
    assert type(result) is TopoDS_Edge

