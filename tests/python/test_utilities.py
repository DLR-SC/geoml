from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt
from OCC.Core.TopoDS import TopoDS_Edge
from OCC.Core.TopAbs import TopAbs_EDGE

import pytest 


###########################################################################
##################### test geoml/utilities/utilities.h #################### 
###########################################################################


def test_utilities_h():
    # test: Array2d<gp_Pnt> extract_control_points_surface(const Handle(Geom_BSplineSurface) &b_spline_surface);
    
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

    surface = pygeoml.interpolate_curves(containers.geomcurve_vector(curve_list), 2, True)

    array_2d = pygeoml.extract_control_points_surface(surface)
    
    cp_22 = surface.Pole(2, 2)

    assert array_2d.at(1, 1).X() == cp_22.X()
    assert array_2d.at(1, 1).Y() == cp_22.Y()
    assert array_2d.at(1, 1).Z() == cp_22.Z()


    # test: std::vector<gp_Pnt> extract_control_point_vector_in_U_direction (const Handle(Geom_BSplineSurface) &b_spline_surface, int j);

    u_vec = pygeoml.extract_control_point_vector_in_U_direction(surface, 1)

    assert u_vec[1].X() == cp_22.X()
    assert u_vec[1].Y() == cp_22.Y()
    assert u_vec[1].Z() == cp_22.Z()


    # test: std::vector<gp_Pnt> extract_control_point_vector_in_V_direction (const Handle(Geom_BSplineSurface) &b_spline_surface, int i);

    v_vec = pygeoml.extract_control_point_vector_in_V_direction(surface, 1)

    assert v_vec[1].X() == cp_22.X()
    assert v_vec[1].Y() == cp_22.Y()
    assert v_vec[1].Z() == cp_22.Z()


    # test: Shape make_fillet (Shape const& solid , Shape const& edges, Standard_Real radius);
    dx = 1.0
    dy = 2.0
    dz = 4.0

    box_shape = pygeoml.create_box(dx, dy, dz)

    pred = pygeoml.is_type(TopAbs_EDGE)

    edge_shapes = box_shape.select_subshapes(pred)

    radius = 0.2

    filleted_box = pygeoml.make_fillet(box_shape, edge_shapes, radius)
    assert type(filleted_box) is pygeoml.Shape

