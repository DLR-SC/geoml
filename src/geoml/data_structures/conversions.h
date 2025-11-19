#pragma once

#include "geoml/data_structures/Array2d.h"

#include <geoml/geoml.h>

#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>   
#include <NCollection_Array1.hxx>
#include <TColStd_Array2OfReal.hxx>

#include <vector>


namespace geoml 
{

/**
 * @brief Convert an Array2d<T> to NCollection_Array2<T>. We remark as an example that the commonly used class TColgp_Array2OfPnt is a typedef of NCollection_Array2<gp_Pnt>.
 * 
 * 
 * @param net An Array2d<t>.
 */
template <typename T>
NCollection_Array2<T> Array2d_to_TCol (Array2d<T> const &net)
{
    NCollection_Array2<T> net_col (1, net.rowLength(), 1, net.colLength());

    for(int i = 0; i < net.rowLength(); ++i)
    {
        for(int j = 0; j < net.colLength(); ++j)
        {
            net_col.SetValue(i + 1, j + 1, net.at(i,j));
        }
    }

    return net_col;
}

inline TColStd_Array2OfReal conversion_func(geoml::Array2d<Standard_Real> const& array_2d) {
    return geoml::Array2d_to_TCol (array_2d);
} 

/**
 * @brief Convert an NCollection_Array2<T> to an Array2d<T>. We remark as an example that the commonly used class TColgp_Array2OfPnt is a typedef of NCollection_Array2<gp_Pnt>.
 * 
 * 
 * @param net A NCollection_Array2<T>.
 */
template <typename T>
Array2d<T> TCol_to_Array2d (NCollection_Array2<T>  const &net)
{
    Array2d<T> array_2d (net.ColLength(), net.RowLength());

    for(int i = 0; i < net.ColLength(); ++i)
    {
        for(int j = 0; j < net.RowLength(); ++j)
        {
            array_2d.setValue(i,j,net.Value(i+1, j+1));
        }
    }

    return array_2d;
}

/**
 * @brief Convert an std::vector<T> to NCollection_Array1<T>. We remark as an example that the commonly used class TColgp_Array1OfPnt is a typedef of NCollection_Array1<gp_Pnt>.
 * 
 *  
 * @param vec An std::vector<T>. 
 */
template <typename T>
NCollection_Array1<T> StdVector_to_TCol(std::vector<T> const &vec)
{
    NCollection_Array1<T> vec_col (1, vec.size());

    for(int i = 0; i < vec.size(); ++i)
    {
            vec_col.SetValue(i + 1, vec.at(i));
    }

    return vec_col;
}

/**
 * @brief Convert a NCollection_Array1<T> to an std::vector<T>. We remark as an example that the commonly used class TColgp_Array1OfPnt is a typedef of NCollection_Array1<gp_Pnt>.
 * 
 *  
 * @param array A NCollection_Array1<T>. 
 */
template <typename T>
std::vector<T> TCol_to_StdVector(NCollection_Array1<T> const &array)
{
    std::vector<T> vec;
    vec.reserve(array.Size());

    for(int i = 0; i < array.Size(); ++i)
    {
        vec.push_back(array.Value(i + 1));
    }

    return vec;
}

} // end namespace geoml