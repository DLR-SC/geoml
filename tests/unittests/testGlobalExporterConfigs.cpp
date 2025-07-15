/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2018 German Aerospace Center (DLR)
*
* Created: 2018-04-04 Martin Siggel <Martin.Siggel@dlr.de>
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
