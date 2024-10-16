/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
*/

#include "gtest/gtest.h"

#include <geoml/geoml.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    geoml::init();

    int retval = RUN_ALL_TESTS();
    return retval;
}
