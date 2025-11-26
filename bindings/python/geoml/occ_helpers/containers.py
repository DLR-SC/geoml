from OCC.Core.TColStd import TColStd_HArray1OfReal, TColStd_HArray1OfInteger, TColStd_HArray2OfReal, TColStd_Array2OfReal
from OCC.Core.TColgp import TColgp_HArray1OfPnt, TColgp_HArray2OfPnt
from OCC.Core.TColGeom import TColGeom_HArray1OfBSplineCurve
from OCC.Core.gp import gp_Pnt
from geoml import CurveList, CPointContainer, IntList, StandardRealList
from geoml import pygeoml
from geoml.pygeoml import Array2dStandardReal, Array2dgp_Pnt


def standard_real_vector(float_list):
    """
    Creates a std::vector of Standard_Real from
    a list of floats
    :param float_list: List of float values
    :return: std::vector<Standard_Real>
    """

    vec = StandardRealList()
    for value in float_list:
        vec.push_back(value)

    return vec 


def int_vector(int_list):
    """
    Creates a std::vector of int from
    a list of integers
    :param int_list: List of integer values
    :return: std::vector<int>
    """

    vec = IntList()
    for value in int_list:
        vec.push_back(value)

    return vec


def float_array(float_list):
    """
    Creates an OpenCASCADE TColStd_HArray1OfReal from
    a list of floats
    :param float_list: List of integer values
    :return: TColStd_HArray1OfReal
    """

    result = TColStd_HArray1OfReal(1, len(float_list))
    for i, value in enumerate(float_list):
        result.SetValue(i+1, value)
    return result


def int_array(int_list):
    """
    Creates an OpenCASCADE TColStd_HArray1OfInteger from
    a list of integers
    :param int_list: List of integer values
    :return: TColStd_HArray1OfInteger
    """
    
    result = TColStd_HArray1OfInteger(1, len(int_list))
    for i, value in enumerate(int_list):
        result.SetValue(i+1, value)
    return result


def point_array(points):
    """
    Returns a TColgp_HArray1OfPnt from a list
    of points given as arrays / tuples

    Ex. points = [(0,0,0), (1,2,3)]

    Note, each point of the array must be 3-dimensional!

    :param points: list of points
    :return: TColgp_HArray1OfPnt
    """

    pnts = TColgp_HArray1OfPnt(1, len(points))
    for i, py_pnt in enumerate(points):
        pnt = gp_Pnt(*py_pnt)
        pnts.SetValue(i+1, pnt)
    return pnts

def float_array_2d_to_tcol (array_2d: Array2dStandardReal):
    """
    Returns a TColgp_HArray2OfReal from a Array2d<Standard_Real>

    :param array_2d: 2d array of type Array2dStandardReal
    :return: TColgp_HArray2OfReal
    """
    
    nrows = array_2d.rowLength()
    ncols = array_2d.colLength()

    TCol_array_2d = TColStd_HArray2OfReal(1, nrows, 1, ncols)
    
    for i in range(nrows):
        for j in range(ncols):
            value = array_2d.at(i,j)
            TCol_array_2d.SetValue(i + 1, j + 1, value)

    return TCol_array_2d


def point_array_2d_to_tcol (array_2d: Array2dgp_Pnt):
    """
    Returns a TColgp_HArray2OfPnt from a Array2d<gp_Pnt>

    :param array_2d: 2d array of type Array2dgp_Pnt
    :return: TColgp_HArray2OfPnt
    """    
    
    nrows = array_2d.rowLength()
    ncols = array_2d.colLength()

    TCol_array_2d = TColgp_HArray2OfPnt(1, nrows, 1, ncols)
    
    for i in range(nrows):
        for j in range(ncols):
            value = array_2d.at(i,j)
            TCol_array_2d.SetValue(i + 1, j + 1, value)

    return TCol_array_2d


def float_tcol_to_array_2d(t_col: TColStd_HArray2OfReal):
    """
    Returns a Array2d<Standard_Real> from a TColStd_HArray2OfReal

    :param t_col: 2d array of type TColStd_HArray2OfReal
    :return: Array2dStandard_Real
    """    
    
    nrows = t_col.ColLength()
    ncols = t_col.RowLength()

    array_2d = Array2dStandardReal(nrows, ncols)

    for i in range(nrows):
        for j in range(ncols):
            value = t_col.Value(i + 1, j + 1)
            array_2d.setValue(i, j, value)
    
    return array_2d


def point_tcol_to_array_2d(t_col: TColgp_HArray2OfPnt):
    """
    Returns a Array2d<gp_Pnt> from a TColgp_HArray2OfPnt

    :param t_col: 2d array of type TColgp_HArray2OfPnt
    :return: Array2dgp_Pnt
    """    
    
    nrows = t_col.ColLength()
    ncols = t_col.RowLength()

    array_2d = Array2dgp_Pnt(nrows, ncols)

    for i in range(nrows):
        for j in range(ncols):
            value = t_col.Value(i + 1, j + 1)
            array_2d.setValue(i, j, value)
    
    return array_2d


def float_vector_to_tcol(vec: StandardRealList):
    """
    Returns a TColStd_HArray1OfReal from a StandardRealList

    :param vec: array of type StandardRealList
    :return: TColStd_HArray1OfReal
    """
    
    length = vec.size()

    result = TColStd_HArray1OfReal(1, length)

    for i in range(length):
        value = vec[i]
        result.SetValue(i+1, value)

    return result


def point_vector_to_tcol(vec: CPointContainer):
    """
    Returns a TColgp_HArray1OfPnt from a CPointContainer

    :param vec: array of type CPointContainer
    :return: TColgp_HArray1OfPnt
    """

    length = vec.size()

    result = TColgp_HArray1OfPnt(1, length)

    for i in range(length):
        value = vec[i]
        result.SetValue(i+1, value)

    return result


def float_tcol_to_vector(t_col: TColStd_HArray1OfReal):
    """
    Returns a StandardRealList from a TColStd_HArray1OfReal

    :param vec: array of type TColStd_HArray1OfReal
    :return: StandardRealList
    """

    length = t_col.Length()

    result = StandardRealList()
    for i in range(length):
        value = t_col.Value(i + 1)
        result.push_back(value)

    return result


def point_tcol_to_vector(t_col: TColgp_HArray1OfPnt):
    """
    Returns a CPointContainer from a TColgp_HArray1OfPnt

    :param vec: array of type TColgp_HArray1OfPnt
    :return: CPointContainer
    """

    length = t_col.Length()

    result = CPointContainer()
    for i in range(length):
        value = t_col.Value(i + 1)
        result.push_back(value)

    return result


def point_vector(gp_pnt_list):
    """
    Creates a std::vector of gp_Pnt to be passed
    to different geometry algorithms of geoml

    :param gp_pnt_list: list of gp_Pnt
    :return: Vectorgp_Pnt
    """

    vec = CPointContainer()
    for pnt in gp_pnt_list:
        vec.push_back(pnt)

    return vec 


def bspline_array(bspline_list):
    """
    Create a TColGeom_HArray1OfBSplineCurve from a list of Geom_BSplineCurves
    :param bspline_list: list of TColGeom_HArray1OfBSplineCurve
    :return: TColGeom_HArray1OfBSplineCurve
    """

    result = TColGeom_HArray1OfBSplineCurve(1, len(bspline_list))
    for i, curve in enumerate(bspline_list):
        result.SetValue(i, curve)
    return result


def geomcurve_vector(geomcurve_list):
    """
    Creates a std::vector of Handle(Geom_Curve) to be passed
    to different geometry algorithms of geoml

    :param geomcurve_list: list of Geom_Curve
    :return: VectorHandleGeom_Curve
    """

    vec = CurveList()
    for curve in geomcurve_list:
        vec.push_back(curve)

    return vec


def iter_array(occ_array):
    """
    Provides a generator for an OpenCASCADE array
    """

    if hasattr(occ_array, "GetObject"):
        occ_array = occ_array.GetObject()

    for i in range(occ_array.Lower(), occ_array.Upper() + 1):
        yield(occ_array.Value(i))
