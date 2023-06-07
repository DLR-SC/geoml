/*
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2012-10-17 Markus Litz <Markus.Litz@dlr.de>
*
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
