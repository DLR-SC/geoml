/* 
* Copyright (C) 2018 German Aerospace Center (DLR/SC)
*
* Created: 2018-04-04 Martin Siggel <Martin.Siggel@dlr.de>
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


#include "test.h"

#include "CGlobalExporterConfigs.h"
#include "ExportStep.h"

TEST(CGlobalExporterConfigs, CheckExporterAvailable)
{
    EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("iges"));
    EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("igs"));
    //EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("vtp"));
    EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("brep"));
    EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("step"));
    EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("stp"));
    EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("step"));
   // EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("dae"));

    //EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("vtk"));
    //EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("collada"));

    EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("STEP"));
    EXPECT_NO_THROW(geoml::CGlobalExporterConfigs::Instance().Get("IGES"));
    
    EXPECT_THROW(geoml::CGlobalExporterConfigs::Instance().Get("unknown"), geoml::Error);
}

TEST(CGlobalExporterConfigs, CheckSameConfig)
{
    geoml::ExporterOptions& stepOptions = geoml::CGlobalExporterConfigs::Instance().Get("step");
    ASSERT_EQ(geoml::NAMED_COMPOUNDS, stepOptions.Get<geoml::ShapeGroupMode>("ShapeGroupMode"));
    ASSERT_EQ(geoml::NAMED_COMPOUNDS,
              geoml::CGlobalExporterConfigs::Instance().Get("stp").Get<geoml::ShapeGroupMode>("ShapeGroupMode"));

    stepOptions.SetShapeGroupMode(geoml::WHOLE_SHAPE);
    ASSERT_EQ(geoml::WHOLE_SHAPE, stepOptions.Get<geoml::ShapeGroupMode>("ShapeGroupMode"));
    ASSERT_EQ(geoml::WHOLE_SHAPE,
              geoml::CGlobalExporterConfigs::Instance().Get("stp").Get<geoml::ShapeGroupMode>("ShapeGroupMode"));

    // This is a global config, hence we have to reset it!
    geoml::CGlobalExporterConfigs::Instance().Get("stp") = geoml::StepOptions();
}
