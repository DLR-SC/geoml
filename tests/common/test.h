/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2012-10-17 Markus Litz <Markus.Litz@dlr.de>
*/
#include <gtest/gtest.h>
#include "geoml_internal.h"
#include <stdexcept>
#include <cmath>

/*
 * Define tests here
 *      
 */


struct InTolerance
{
    InTolerance(double tol) : m_tol(tol) {}
    InTolerance(const InTolerance& other) : m_tol(other.m_tol) {}

    bool operator()(double v1, double v2) const
    {
        return fabs(v1 - v2) <= m_tol;
    }

    double m_tol;
};



template <typename Array, class Compare>
::testing::AssertionResult ArraysMatch(const Array& expected,
                                       const Array& actual,
                                       Compare is_equal)
{
    
    if (expected.size() != actual.size()) {
        return ::testing::AssertionFailure() << "Expected size (" << expected.size() << ") != actual size (" << actual.size() <<  ")";
    }
    
    
    auto it1 = std::begin(expected);
    auto it2 = std::begin(actual);
    int idx = 0;
    while(it1 != std::end(expected) && it2 != std::end(actual)){
        if (!is_equal(*it1, *it2)){
            return ::testing::AssertionFailure() << "array[" << idx
                                                 << "] (" << *it2 << ") != expected[" << idx
                                                 << "] (" << *it1 << ")";
        }
        ++idx;
        ++it1;
        ++it2;
    }
    
    return ::testing::AssertionSuccess();
}

template <typename Array>
::testing::AssertionResult ArraysMatch(const Array& expected,
                                       const Array& actual)
{
    using T = typename Array::value_type;

    return ArraysMatch(expected, actual, [](const T& v1, const T& v2) {
        return v1 == v2;
    });
}


class TiGLTestError : public std::runtime_error
{
public:
    TiGLTestError(const std::string& what) : std::runtime_error(what)
    {}
};
