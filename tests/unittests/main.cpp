/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2012-10-17 Markus Litz <Markus.Litz@dlr.de>
*/

#include "gtest/gtest.h"
#include "logging/Logging.h"
#include <geoml/geoml.h>


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    geoml::init();

    geoml::Logging::Instance().SetTimeIdInFilenameEnabled(false);
    // disable any console logging
    geoml::Logging::Instance().SetConsoleVerbosity(geoml::LogLevel_SILENT);
    geoml::Logging::Instance().LogToFile("geomltest");

    int retval = RUN_ALL_TESTS();
    return retval;
}
