/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-05-27 Martin Siggel <Martin.Siggel@dlr.de>
*/


#include "test.h"
#include "imports/StepReader.h"
#include "CNamedShape.h"
#include "imports/ImporterFactory.h"

TEST(Import, Step)
{
    geoml::ImportStep reader;
    ListPNamedShape shapes = reader.Read("TestData/nacelle.stp");
    ASSERT_EQ(1, shapes.size());

    // test shape names
    ASSERT_STREQ("Nacelle", shapes[0]->Name().c_str());
}

TEST(Import, ImporterFactory)
{
    geoml::ImporterFactory factory = geoml::ImporterFactory::Instance();

    auto importer = factory.Create("step");
    ASSERT_TRUE(importer != NULL);
    ASSERT_STREQ("step", importer->SupportedFileType().c_str());

    importer = factory.Create("STEP");
    ASSERT_TRUE(importer != NULL);
    ASSERT_STREQ("step", importer->SupportedFileType().c_str());

    importer = factory.Create("invalidformat");
    ASSERT_TRUE(importer == NULL);
    
    ASSERT_TRUE(factory.ImporterSupported("step"));
    ASSERT_FALSE(factory.ImporterSupported("invalidformat"));
}
