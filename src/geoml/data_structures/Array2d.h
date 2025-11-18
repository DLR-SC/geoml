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

#include <geoml/geoml.h>  

#include <vector>

namespace geoml 
{

template<typename T>
class Array2d
{
public:
    // Remark: We will use the following mapping to map a 2d-index to the corresponding 1d-index in 
    // the flattened vector: (i,j) -> i * m + j
    // The inverse mapping is given by: k -> ( floor(k / m_rows), k - floor(k / m) * m ) (or a reformulation using division with remainder)
    GEOML_API_EXPORT Array2d(int rows, int cols) : m_rows{rows}, m_cols{cols}, m_flat_data(m_rows * m_cols) {}

    GEOML_API_EXPORT T& at(int row, int col) {
        return m_flat_data.at(row * m_cols + col);
    }
    
    GEOML_API_EXPORT const T& at(int row, int col) const {
        return m_flat_data.at(row * m_cols + col);
    } 
    
    GEOML_API_EXPORT T& operator()(int row, int col) {
        return m_flat_data[row * m_cols + col];
    }

    GEOML_API_EXPORT const T& operator()(int row, int col) const {
        return m_flat_data[row * m_cols + col];
    }
       
    GEOML_API_EXPORT void setValue(int row, int col, T input) {
        m_flat_data.at(row * m_cols + col) = input;
    }

    GEOML_API_EXPORT int rowLength() const {
        return m_rows;
    }

    GEOML_API_EXPORT int colLength() const {
        return m_cols;
    }

    GEOML_API_EXPORT std::vector<T> getRow(int index){
        std::vector<T> row;
        row.reserve(rowLength());
        for(int j = 0; j < m_cols; ++j){
            row.push_back(this->at(index, j));
        }

        return row;
    }

    GEOML_API_EXPORT std::vector<T> getCol(int index){
        std::vector<T> col;
        col.reserve(colLength());
        for(int i = 0; i < m_rows; ++i){
            col.push_back(this->at(index, i));
        }

        return col;
    }

private:
    int m_rows;
    int m_cols;
    std::vector<T> m_flat_data;

};

} // end namespace geoml

