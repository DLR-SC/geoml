#pragma once

#include "geoml/data_structures/Array2d.h"

#include <geoml/geoml.h>

#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>   

namespace geoml 
{


/**
 * @brief Convert an Array2d<T> to NCollection_Array2<T>. We remark that the commonly used class TColgp_Array2OfPnt is a typedef of NCollection_Array2<T>.
 * 
 * 
 * @param net A two dimensional array of objects of type T.
 */
template <typename T>
GEOML_API_EXPORT  NCollection_Array2<T> Array2d_to_TCol(Array2d<T> const &net)
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

} // end namespace geoml