from geoml import pygeoml
from OCC.Core.gp import gp_Pnt

import pytest 

    
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

    # this does not work yet! See containers.py for a workaround instead.

