from geoml import pygeoml
from geoml.pygeoml import is_vertex, is_edge
from geoml.occ_helpers import containers
from OCC.Core.TColStd import TColStd_Array2OfReal, TColStd_HArray2OfReal
from OCC.Core.gp import gp_Pnt, gp_Vec
from OCC.Core.TColgp import TColgp_HArray1OfPnt, TColgp_HArray2OfPnt

import pytest 


###########################################################################
##################### test geoml/occ_helpers/containers.py ################
###########################################################################

def test_standard_real_vector():
    float_list = [1.0, 2.0, 3.0, 4.0]

    vec = containers.standard_real_vector(float_list)

    assert vec[1] == 2.0
    assert vec[2] == 3.0

def test_int_vector():
    int_list = [1, 2, 3, 4] 

    vec = containers.int_vector(int_list)

    assert vec[1] == 2   
    assert vec[2] == 3   

def test_point_vector():
    p_1 = gp_Pnt(0.0,0.0,0.0)
    p_2 = gp_Pnt(0.5,0.0,0.0)       
    p_3 = gp_Pnt(1.0,1.0,0.0)

    point_list = [p_1, p_2, p_3]

    result = containers.point_vector(point_list)
    
    assert type(result) is pygeoml.CPointContainer


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
    
