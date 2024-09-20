/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2018 German Aerospace Center (DLR)
*
* Created: 2018-11-23 Martin Siggel <martin.siggel@dlr.de>
*/

#include "test.h"

#include "sorting.h"

bool int_follows(int v2, int v1)
{
    return v2 == v1 + 1;
}


TEST(Sorting, FollowSort)
{
    std::vector<int> v{5, 3, 8, 2,9, 1, 7, 0, 4, 6};

    ASSERT_NO_THROW(geoml::follow_sort(v.begin(), v.begin()+ 10, int_follows));

    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(static_cast<int>(i), v[i]);
    }

    v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ASSERT_NO_THROW(geoml::follow_sort(v.begin(), v.begin()+ 10, int_follows));

    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(static_cast<int>(i), v[i]);
    }

    v = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

    ASSERT_NO_THROW(geoml::follow_sort(v.begin(), v.begin()+ 10, int_follows));

    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(static_cast<int>(i), v[i]);
    }

    v = {5, 3, 8, 2,9, 1, 7, 0, 4, 6};
    ASSERT_THROW(geoml::follow_sort(v.begin(), v.begin()+ 5, int_follows), std::invalid_argument);

    v = {1, 2, 3, 4, 5, 1, 7, 0, 4, 6};
    ASSERT_NO_THROW(geoml::follow_sort(v.begin(), v.begin()+ 5, int_follows));

    v = {5, 4, 3, 2, 1, 1, 7, 0, 4, 6};
    ASSERT_NO_THROW(geoml::follow_sort(v.begin(), v.begin()+ 5, int_follows));

    std::vector<int> expected = {1, 2, 3, 4, 5, 1, 7, 0, 4, 6};

    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(expected[i], v[i]);
    }
}


TEST(Sorting, RotateRight)
{
    std::vector<int> v{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ASSERT_NO_THROW(geoml::rotate_right(v.begin(), v.begin() + 4));

    std::vector<int> expected =  {4, 0, 1, 2, 3, 5, 6, 7, 8, 9};

    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_EQ(expected[i], v[i]);
    }
}
