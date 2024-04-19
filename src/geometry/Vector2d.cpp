#include "Vector2d.h"

namespace geoml 
{

template<typename T>
Vector2d<T>::Vector2d(int rows, int cols) : m_rows{rows}, m_cols{cols}, m_flat_data(m_rows * m_cols) {}


} // namespace geoml


// Remark: We will use the following mapping to map a 2d-index to the corresponding 1d-index in 
//         the flattened vector: (i,j) -> (i + 1) * m - (m - (j + 1)) - 1
//         The inverse mapping is given by: k -> ( floor(k / m_rows), k - floor(k / m) * m )
