#include Array2d.h

namespace geoml{

GEOML_API_EXPORT TColgp_Array2OfPnt convertArray2d_gp_Pnt_to_TCol(Array2d<gp_Pnt> &point_net)
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
} // namespace geoml

