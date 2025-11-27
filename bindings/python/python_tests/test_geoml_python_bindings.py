from geoml import pygeoml
from geoml.pygeoml import is_vertex, is_edge
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt
from OCC.Core.gp import gp_Vec
from OCC.Core.TopoDS import TopoDS_Shape
from OCC.Display.SimpleGui import init_display 
from OCC.Core.Geom import Geom_BSplineCurve, Geom_Curve, Geom_BSplineSurface
from OCC.Core.TopAbs import TopAbs_EDGE, TopAbs_FACE, TopAbs_VERTEX, TopAbs_SHAPE
from OCC.Core.TopAbs import TopAbs_ShapeEnum
from OCC.Core.BRepPrimAPI import BRepPrimAPI_MakeBox, BRepPrimAPI_MakeSphere
from OCC.Core.TColStd import TColStd_Array2OfReal, TColStd_HArray2OfReal
from OCC.Core.TColgp import TColgp_HArray1OfPnt, TColgp_HArray2OfPnt

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
##################### test geoml/occ_helpers/containers.py ################
###########################################################################


def compute_point_vector():
    p_1 = gp_Pnt(0.0,0.0,0.0)
    p_2 = gp_Pnt(0.5,0.0,0.0)       
    p_3 = gp_Pnt(1.0,1.0,0.0)

    point_list = [p_1, p_2, p_3]

    return containers.point_vector(point_list)


def test_point_vector():

    func_name = "point_vector"
          
    try:
        result = compute_point_vector()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


def test_float_array_2d_to_tcol():
    # test: def float_array_2d_to_tcol (array_2d: Array2dStandardReal):

    weights = pygeoml.Array2dStandardReal(3, 3)

    w_11 = 1.0 
    w_12 = 2.0       
    w_13 = 1.0

    w_21 = 1.0
    w_22 = 1.0      
    w_23 = 1.0

    w_31 = 1.0
    w_32 = 1.0
    w_33 = 1.0

    weights.setValue(0, 0, w_11)
    weights.setValue(0, 1, w_12)
    weights.setValue(0, 2, w_13)
    weights.setValue(1, 0, w_21)
    weights.setValue(1, 1, w_22)
    weights.setValue(1, 2, w_23)
    weights.setValue(2, 0, w_31)
    weights.setValue(2, 1, w_32)
    weights.setValue(2, 2, w_33)  

    TCol_weights =  containers.float_array_2d_to_tcol(weights)

    assert TCol_weights.Value(1,2) == 2.0


def test_point_array_2d_to_tcol():
    # test: def point_array_2d_to_tcol (array_2d: Array2dgp_Pnt):

    points = pygeoml.Array2dgp_Pnt(3, 3)

    p_11 = gp_Pnt(1., 0., 0.)
    p_12 = gp_Pnt(1., 1., 0.)
    p_13 = gp_Pnt(1., 2., 0.)

    p_21 = gp_Pnt(2., 0., 1.)
    p_22 = gp_Pnt(2., 1., 1.)      
    p_23 = gp_Pnt(2., 2., 1.)

    p_31 = gp_Pnt(3., 0., 3.)
    p_32 = gp_Pnt(3., 1., 3.)
    p_33 = gp_Pnt(3., 2., 3.)

    points.setValue(0, 0, p_11)
    points.setValue(0, 1, p_12)
    points.setValue(0, 2, p_13)
    points.setValue(1, 0, p_21)
    points.setValue(1, 1, p_22)
    points.setValue(1, 2, p_23)
    points.setValue(2, 0, p_31)
    points.setValue(2, 1, p_32)
    points.setValue(2, 2, p_33)  

    TCol_points =  containers.point_array_2d_to_tcol(points)

    assert TCol_points.Value(1,2).X() == 1.0
    assert TCol_points.Value(1,2).Y() == 1.0
    assert TCol_points.Value(1,2).Z() == 0.0
    

def test_float_tcol_to_array_2d():
    # test: def float_tcol_to_array_2d(t_col: TColStd_HArray2OfReal):
     
    TCol_array_2d = TColStd_HArray2OfReal(1, 2, 1, 2)

    TCol_array_2d.SetValue(1, 1, 1.0)
    TCol_array_2d.SetValue(1, 2, 2.0)
    TCol_array_2d.SetValue(2, 1, 3.0)
    TCol_array_2d.SetValue(2, 2, 4.0)

    array_2d = containers.float_tcol_to_array_2d(TCol_array_2d)

    assert array_2d.at(1, 1) == 4.0


def test_point_tcol_to_array_2d():
    # test: def point_tcol_to_array_2d(t_col: TColgp_HArray2OfPnt):
     
    TCol_array_2d = TColgp_HArray2OfPnt(1, 3, 1, 3)

    p_11 = gp_Pnt(1., 0., 0.)
    p_12 = gp_Pnt(1., 1., 0.)
    p_13 = gp_Pnt(1., 2., 0.)

    p_21 = gp_Pnt(2., 0., 1.)
    p_22 = gp_Pnt(2., 1., 1.)      
    p_23 = gp_Pnt(2., 2., 1.)

    p_31 = gp_Pnt(3., 0., 3.)
    p_32 = gp_Pnt(3., 1., 3.)
    p_33 = gp_Pnt(3., 2., 3.)

    TCol_array_2d.SetValue(1, 1, p_11)
    TCol_array_2d.SetValue(1, 2, p_12)
    TCol_array_2d.SetValue(1, 3, p_13)
    TCol_array_2d.SetValue(2, 1, p_21)
    TCol_array_2d.SetValue(2, 2, p_22)
    TCol_array_2d.SetValue(2, 3, p_23)
    TCol_array_2d.SetValue(3, 1, p_31)
    TCol_array_2d.SetValue(3, 2, p_32)
    TCol_array_2d.SetValue(3, 3, p_33)

    array_2d = containers.point_tcol_to_array_2d(TCol_array_2d)

    assert array_2d.at(1, 2).X() == 2.0
    assert array_2d.at(1, 2).Y() == 2.0
    assert array_2d.at(1, 2).Z() == 1.0


def test_float_vector_to_tcol():
    # test: NCollection_Array1<Standard_Real> StdVector_to_TCol(std::vector<Standard_Real> const &vec)
    
    vec = containers.standard_real_vector([1.0, 2.0, 3.0, 4.0])
    t_col = containers.float_vector_to_tcol(vec)

    assert t_col.Value(2) == 2.0


def test_point_vector_to_tcol():
    # test: NCollection_Array1<gp_Pnt> StdVector_to_TCol(std::vector<gp_Pnt> const &vec)

    vec = containers.point_vector([gp_Pnt(1.0, 2.0, 3.0), gp_Pnt(4.0, 5.0, 6.0), gp_Pnt(7.0, 8.0, 9.0)])
    t_col = containers.point_vector_to_tcol(vec)

    assert t_col.Value(1).X() == 1.0
    assert t_col.Value(2).Y() == 5.0


def test_float_tcol_to_vector():
    # test: std::vector<Standard_Real> TCol_to_StdVector(NCollection_Array1<Standard_Real> const &array)

    t_col = containers.float_array([1.0, 2.0, 3.0, 4.0])
    vec = containers.float_tcol_to_vector(t_col)

    assert vec[0] == 1.0
    assert vec[1] == 2.0


def test_point_tcol_to_vector():
    # test: std::vector<gp_Pnt> TCol_to_StdVector(NCollection_Array1<gp_Pnt> const &array)

    t_col = containers.point_array([(1.0, 2.0, 3.0), (4.0, 5.0, 6.0), (7.0, 8.0, 9.0)])
    vec = containers.point_tcol_to_vector(t_col)

    assert vec[0].X() == 1.0
    assert vec[1].Y() == 5.0
    

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

    func_name = "nurbs_curve"

    try:
        result = compute_nurbs_curve()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


def compute_interpolate_points_to_b_spline_curve():
     
    p_1 = gp_Pnt(0,0,0)
    p_2 = gp_Pnt(0.5,0,0)       
    p_3 = gp_Pnt(1,1,0)

    point_list = [p_1, p_2, p_3]

    curve = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list), 2)
    
    return curve


def test_interpolate_points_to_b_spline_curve():

    func_name = "interpolate_points_to_b_spline_curve"

    try:
        result = compute_interpolate_points_to_b_spline_curve()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


###########################################################################
##################### test geoml/curves/BlendCurve.h ##########################
###########################################################################


def compute_blend_curve():

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

    func_name = "BlendCurve"

    try:
        result = compute_blend_curve()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


###########################################################################
##################### test geoml/surfaces/surfaces.h ######################
###########################################################################


def compute_curve_network_interpolation():

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


def test_curve_network_interpolation():
    
    func_name = "curve_network_interpolation"

    try:
        result = compute_curve_network_interpolation()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


def compute_interpolate_curves():

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

    return surface


def test_interpolate_curves():

    func_name = "interpolate_curves"

    try:
        result = compute_interpolate_curves()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}") 


def compute_revolving_shape():

    p_1 = gp_Pnt(2.0, 0.0, 0.0)
    p_2 = gp_Pnt(1.0, 0.0, 1.0)       
    p_3 = gp_Pnt(3.0, 0.0, 2.0)

    point_list = [p_1, p_2, p_3]

    curve = pygeoml.interpolate_points_to_b_spline_curve(containers.point_vector(point_list), 2)
    edge = pygeoml.CurveToEdge(curve)

    start_point_rotation_axis = gp_Pnt(0.0, 0.0, 0.0)

    rotation_axis_direction = gp_Vec(0.0, 0.0, 1.0)

    revolving_shape = pygeoml.revolving_shape(edge, start_point_rotation_axis, rotation_axis_direction)

    return revolving_shape


def test_revolving_shape():

    func_name = "revolving_surface"

    try:
        result = compute_revolving_shape()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}") 


def compute_nurbs_surface():

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

    surface = pygeoml.nurbs_surface(control_points, weights, 
                                    containers.standard_real_vector(u_knots), 
                                    containers.standard_real_vector(v_knots), 
                                    containers.int_vector(u_mults),
                                    containers.int_vector(v_mults),
                                    u_degree, v_degree)
    
    return surface


def test_nurbs_surface():

    func_name = "nurbs_surface"

    try:
        result = compute_nurbs_surface()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}") 


def compute_create_surface():
    x = gp_Pnt(0,0,0)
    y = gp_Pnt(0.5,0,0)       
    z = gp_Pnt(1,1,0)
    w = gp_Pnt(0,3,0)
    
    return pygeoml.create_surface(x,y,z,w)


def test_create_surface():

    func_name = "create_surface"

    try:
        result = compute_create_surface()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


def compute_create_face():
    x = gp_Pnt(0,0,0)
    y = gp_Pnt(0.5,0,0)       
    z = gp_Pnt(1,1,0)
    w = gp_Pnt(0,3,0)
    
    return pygeoml.create_face(x,y,z,w)


def test_create_face():

    func_name = "create_face"

    try:
        result = compute_create_face()
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


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
    try:
        wrapped_shape = edge_shape.shape()
    except Exception as e:
        pytest.fail(f"Calling {'shape'} raised an exception: {e}")
    
    # wrapped_shapet: operator TopoDS_Shape() const;
    # -> does not work currently

    # test: std::vector<Shape>& direct_subshapes();
    # sub_shapes = edge_shape.direct_subshapes()
    try:
        edge_shape.direct_subshapes()
    except Exception as e:
        pytest.fail(f"Calling {'direct_subshapes'} raised an exception: {e}")

    # test: Shape get_subshapes() const;
    try:
        edge_shape.get_subshapes()
    except Exception as e:
            pytest.fail(f"Calling {'get_subshapes'} raised an exception: {e}")

    # test: void add_meta_tag(std::string const& tag);
    try:
        edge_shape.add_meta_tag("added_tag")
    except Exception as e:
            pytest.fail(f"Calling {'add_meta_tag'} raised an exception: {e}")

    # test: template <typename Pred>
    # Shape select_subshapes(Pred&& f, int max_depth = std::numeric_limits<int>::max()) const
    pred_1 = pygeoml.has_tag("not_added_tag")
    pred_2 = pygeoml.has_tag("added_tag")

    try:
        edge_shape.select_subshapes(pred_1)
        edge_shape.select_subshapes(pred_2)
    except Exception as e:
            pytest.fail(f"Calling {'select_subshapes'} raised an exception: {e}")

    # test: template <typename Pred>
    # Shape filter(Pred&& f) const
    try:
        edge_shape.filter(pred_2)
    except Exception as e:
            pytest.fail(f"Calling {'filter'} raised an exception: {e}")

    # test: Shape unique_element() const;
    # edge_shape.unique_element() #get an error, probably as expected

    # test: Shape unique_element_or(Shape const& other) const;
    try:
        edge_shape.unique_element_or(edge_shape)
    except Exception as e:
            pytest.fail(f"Calling {'unique_element'} raised an exception: {e}")

    # test: template <typename Pred>
    # void add_meta_tag_to_subshapes(Pred&& f, std::string const& input_tag)
    try:
        edge_shape.add_meta_tag_to_subshapes(pred_2, "another_added_tag")
    except Exception as e:
            pytest.fail(f"Calling {'add_meta_tag_to_subshapes'} raised an exception: {e}")

    # test: void add_tag_track(TagTrack const& tt);
    tag_track = pygeoml.TagTrack("added_by_tag_track_if_is_edge", is_edge, 2)
    
    try:
        edge_shape.add_tag_track(tag_track)
    except Exception as e:
            pytest.fail(f"Calling {'add_tag_track'} raised an exception: {e}")

    # test: std::vector<TagTrack>& get_tag_tracks();
    try:
        tag_tracks = edge_shape.get_tag_tracks()
    except Exception as e:
            pytest.fail(f"Calling {'edge_shape'} raised an exception: {e}")

    # test: void apply_tag_tracks();
    try:
        edge_shape.apply_tag_tracks()
    except Exception as e:
            pytest.fail(f"Calling {'apply_tag_tracks'} raised an exception: {e}")

    assert edge_shape.has_tag("not_added_tag") == False
    assert edge_shape.has_tag("added_tag") == True
    assert edge_shape.has_tag("another_added_tag") == True
    assert edge_shape.has_tag("added_by_tag_track_if_is_edge") == True

    # test: Shape operator[](int i);
    try:
        edge_shape[0]
        edge_shape[1]
        edge_shape[2]
        edge_shape[3]
        edge_shape[4]
        edge_shape[5]
        edge_shape[6]
    except Exception as e:
            pytest.fail(f"Calling {'operator[](int i)'} raised an exception: {e}")

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

    # test: bool _is_vertex(Shape const& s){ return s.is_type(TopAbs_VERTEX); }
    assert pygeoml._is_vertex(edge_shape) == False

    # test: bool _is_edge(Shape const& s){ return s.is_type(TopAbs_EDGE); }
    assert pygeoml._is_edge(edge_shape) == True

    # test: bool _is_face(Shape const& s){ return s.is_type(TopAbs_FACE); }
    assert pygeoml._is_face(edge_shape) == False

    # test: bool _is_solid(Shape const& s){ return s.is_type(TopAbs_SOLID); }
    assert pygeoml._is_solid(edge_shape) == False

    # test: bool _has_origin(Shape const& s){ return s.has_origin(); }
    assert pygeoml._has_origin(edge_shape) == False

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


###########################################################################
##################### test geoml/boolean_ops/modeling.hpp #################
###########################################################################


def test_boolean_ops():

    box = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape()
    sphere = BRepPrimAPI_MakeSphere(5.0).Shape()

    box_shape = pygeoml.Shape(box)
    sphere_shape = pygeoml.Shape(sphere)

    # test: Shape boolean_subtract(Shape const& shape, Shape const& cutting_tool);
    try:
        subtracted_shape = pygeoml.boolean_subtract(box_shape, sphere_shape)
    except Exception as e:
        pytest.fail(f"Calling {'boolean_subtract'} raised an exception: {e}")

    # test: Shape operator-(Shape const& shape, Shape const& cutting_tool);
    try:
        subtracted_shape_1 = sphere_shape - box_shape
    except Exception as e:
        pytest.fail(f"Calling {'operator-'} raised an exception: {e}")
    
    # test: Shape boolean_union(Shape const& shape_1, Shape const& shape_2);
    try:
        united_shape = pygeoml.boolean_union(box_shape, sphere_shape)
    except Exception as e:
        pytest.fail(f"Calling {'boolean_union'} raised an exception: {e}")

    # test: Shape operator+(Shape const& shape_1, Shape const& shape_2);
    try:
        united_shape_1 = box_shape + sphere_shape
    except Exception as e:
        pytest.fail(f"Calling {'operator+'} raised an exception: {e}")

    
###########################################################################
##################### test geoml/data_structures/Array2d.h ################
###########################################################################


def test_Array2d_h():
    
    # test: construct an Array2dStandardReal and set values
    weights = pygeoml.Array2dStandardReal(3, 3)

    w_11 = 1.0 
    w_12 = 2.0       
    w_13 = 1.0

    w_21 = 1.0
    w_22 = 1.0      
    w_23 = 1.0

    w_31 = 1.0
    w_32 = 1.0
    w_33 = 1.0

    try:
        weights.setValue(0, 0, w_11)
        weights.setValue(0, 1, w_12)
        weights.setValue(0, 2, w_13)
        weights.setValue(1, 0, w_21)
        weights.setValue(1, 1, w_22)
        weights.setValue(1, 2, w_23)
        weights.setValue(2, 0, w_31)
        weights.setValue(2, 1, w_32)
        weights.setValue(2, 2, w_33) 
    except Exception as e:
        pytest.fail(f"Calling {'setValue for Array2dStandardReal'} raised an exception: {e}")

    # test: construct an Array2dgp_Pnt and set values
    control_points = pygeoml.Array2dgp_Pnt(3, 3)

    p_11 = gp_Pnt(0.0,0.0,0.0)
    p_12 = gp_Pnt(0.0,1.0,0.0)       
    p_13 = gp_Pnt(1.0,2.0,0.0)

    p_21 = gp_Pnt(0.0,0.0,3.0)
    p_22 = gp_Pnt(0.0,1.0,3.0)       
    p_23 = gp_Pnt(1.0,2.0,3.0)

    p_31 = gp_Pnt(0.0,4.0,3.0)
    p_32 = gp_Pnt(0.0,5.0,3.0)       
    p_33 = gp_Pnt(1.0,6.0,3.0)

    try:
        control_points.setValue(0, 0, p_11)
        control_points.setValue(0, 1, p_12)
        control_points.setValue(0, 2, p_13)
        control_points.setValue(1, 0, p_21)
        control_points.setValue(1, 1, p_22)
        control_points.setValue(1, 2, p_23)
        control_points.setValue(2, 0, p_31)
        control_points.setValue(2, 1, p_32)
        control_points.setValue(2, 2, p_33)
    except Exception as e:
        pytest.fail(f"Calling {'setValue for Array2dgp_Pnt'} raised an exception: {e}")

    # test: T& at(int row, int col)
    value_at_1_1 = control_points.at(1,1)
    assert value_at_1_1.X() == p_22.X()
    assert value_at_1_1.Y() == p_22.Y()
    assert value_at_1_1.Z() == p_22.Z()

    assert value_at_1_1.IsEqual(p_22, 1e-5)

    # other_value_at_1_1 = weights.at(1,1)
    other_value_at_1_1 = weights.at(1,1)
    assert other_value_at_1_1 == w_22 # if one uses .at(1,1) in the line above, this comparison gives an error 

    # test: T& operator()(int row, int col)    
    value_at_1_1_new = control_points(1,1)
    assert value_at_1_1_new.IsEqual(p_22, 1e-5)

    other_value_at_1_1_new = weights(1,1)
    assert other_value_at_1_1_new ==w_22
    
    # test: void setValue(int row, int col, T input)
    weights.setValue(1, 1, 2.2)

    assert weights(1, 1) == 2.2

    # test: int rowLength() const
    assert weights.rowLength() == 3

    # test: int colLength() const
    assert weights.colLength() == 3

    # test: std::vector<T> getRow(int index)
    row = weights.getRow(1) 

    assert row[1] == 2.2 # note, the .at(int i) method of std::vector is not exposed here!

    # test: std::vector<T> getCol(int index)
    col = weights.getCol(1) 
    assert col[0] == 2.0


###########################################################################
##################### test geoml/data_structures/conversions.h ############
###########################################################################


def test_conversions_h():
    weights = pygeoml.Array2dStandardReal(3, 3)

    w_11 = 1.0 
    w_12 = 2.0       
    w_13 = 1.0

    w_21 = 1.0
    w_22 = 1.0      
    w_23 = 1.0

    w_31 = 1.0
    w_32 = 1.0
    w_33 = 1.0

    weights.setValue(0, 0, w_11)
    weights.setValue(0, 1, w_12)
    weights.setValue(0, 2, w_13)
    weights.setValue(1, 0, w_21)
    weights.setValue(1, 1, w_22)
    weights.setValue(1, 2, w_23)
    weights.setValue(2, 0, w_31)
    weights.setValue(2, 1, w_32)
    weights.setValue(2, 2, w_33)     
    
    # test: NCollection_Array2<StandardReal> Array2d_to_TCol (Array2d<StandardReal> const &net)
    

###########################################################################
##################### test geoml/geom_topo_conversions/geom_topo_conversions.h 
###########################################################################


def test_geom_topo_conversions_h():
    # test: TopoDS_Face SurfaceToFace(const Handle(Geom_Surface) &surface, Standard_Real tolDegEd);

    x = gp_Pnt(0,0,0)
    y = gp_Pnt(0.5,0,0)       
    z = gp_Pnt(1,1,0)
    w = gp_Pnt(0,3,0)
    
    srf = pygeoml.create_surface(x,y,z,w)

    func_name = "SurfaceToFace"
    try:
        face = pygeoml.SurfaceToFace(srf, 1e-4)
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")

    # test: TopoDS_Edge CurveToEdge(const Handle(Geom_Curve) &curve);

    p_1 = gp_Pnt(0.0, 0.0, 0.0)
    p_2 = gp_Pnt(1.0, 1.0, 1.0)
    p_3 = gp_Pnt(3.0, 0.0, 6.0)

    point_vec = containers.point_vector([p_1, p_2, p_3])

    curve = pygeoml.interpolate_points_to_b_spline_curve(point_vec)

    func_name = "CurveToEdge"
    try:
        edge = pygeoml.CurveToEdge(curve)
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")
    
    # test: Handle(Geom_Curve) EdgeToCurve(TopoDS_Edge const& edge);

    edge = pygeoml.CurveToEdge(curve)

    func_name = "EdgeToCurve"
    try:
        crv = pygeoml.EdgeToCurve(edge)
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")

###########################################################################
##################### test geoml/primitives/modeling.hpp ################## 
###########################################################################


def test_primitives():
    # test: Shape create_cylinder(double radius, double height);

    radius = 2.0
    height = 8.0

    func_name = "create_cylinder"
    try:
        cylinder_shape = pygeoml.create_cylinder(radius, height)
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")

    # test: Shape create_box(double dx, double dy, double dz);

    dx = 1.0
    dy = 2.0
    dz = 4.0

    func_name = "create_box"
    try:
        box_shape = pygeoml.create_box(dx, dy, dz)
    except Exception as e:
        pytest.fail(f"Calling {func_name} raised an exception: {e}")


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



print("Ran through till the end")
