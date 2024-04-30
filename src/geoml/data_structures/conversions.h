#pragma once

#include "geoml/data_structures/Array2d.h"

#include <geoml/geoml.h>

#include <gp_Pnt.hxx>
#include <TColgp_Array2OfPnt.hxx>   

namespace geoml 
{

template <typename T>
GEOML_API_EXPORT  NCollection_Array2<T> Array2d_to_TCol(Array2d<T> const &point_net)
{
    TColgp_Array2OfPnt points_col (1, point_net.rowLength(), 1, point_net.colLength());

    for(int i = 0; i < point_net.rowLength(); ++i)
    {
        for(int j = 0; j < point_net.colLength(); ++j)
        {
            points_col.SetValue(i + 1, j + 1, point_net.at(i,j));
        }
    }

    return points_col;
}

} // end namespace geoml