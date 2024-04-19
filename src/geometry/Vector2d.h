/* 
* Copyright (C) 2024 German Aerospace Center (DLR/SC)
*
* Created: 2024-04-10 Anton Reiswich <Anton.Reiswich@dlr.de>
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
/**
* @file
* @brief  Implementation of a 2 dimensional data structure.
*/
#pragma once

#include "geoml_internal.h"

#include <vector>

namespace geoml 
{

template<typename T>
class Vector2d
{
public:

    Vector2d(int rows, int cols);

    //T& at(int row, int col);

    //const T& at(int row, int col); 

    //T& operator()(int row, int col);

    //const T& operator()(int row, int col) const;

    //void setValue(int row, int col, T input);

    //int rowLength() const;

    //int colLength() const;

private:

    int m_rows;
    int m_cols;
    std::vector<T> m_flat_data;

};

} // end namespace geoml

