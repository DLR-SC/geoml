/* 
* Copyright (C) 2018 German Aerospace Center (DLR/SC)
*
* Created: 2018-03-29 Martin Siggel <Martin.Siggel@dlr.de>
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

#include "COptionList.h"
#include "any.h"

class MockOptions : public geoml::COptionList
{
public:
    MockOptions()
    {
        AddOption<double>("my_double", 0.0);
        AddOption<std::string>("my_string", "Hallo");
    }
};

class MockOptionsOther : public geoml::COptionList
{
public:
    MockOptionsOther()
    {
        AddOption<int>("my_int", 10);
    }
};

TEST(OptionList, AddGetSet)
{
    MockOptions options;

    Standard_Real v1 = options.Get<double>("my_double");
    EXPECT_NEAR(0.0, v1.getValue(), 1e-10);

    options.SetDouble("my_double", 10.);
    v1 = options.Get<double>("my_double");
    EXPECT_NEAR(10.0, v1.getValue(), 1e-10);

    options.SetFromString("my_double", "20.");
    v1 = options.Get<double>("my_double");
    EXPECT_NEAR(20.0, v1.getValue(), 1e-10);

    std::string v2 = options.Get<std::string>("my_string");
    EXPECT_STREQ("Hallo", v2.c_str());

    options.Set<std::string>("my_string", "welt");
    v2 = options.Get<std::string>("my_string");
    EXPECT_STREQ("welt", v2.c_str());

    options.SetString("my_string", "hello world");
    v2 = options.GetString("my_string");
    EXPECT_STREQ("hello world", v2.c_str());

    // check failures
    // cannot convert
    EXPECT_THROW(options.Set("my_string", 2.0), geoml::Error);

    // no such option
    EXPECT_THROW(options.Set<std::string>("my_new_string", "welt"),
                 geoml::Error);

    // cannot convert
    EXPECT_THROW(options.Get<int>("my_double"), geoml::Error);

    // no such option
    EXPECT_THROW(options.Get<int>("my_new_double"), geoml::Error);

    // cannot convert "welt" to a Standard_Real
    EXPECT_THROW(options.SetFromString("my_doublee", "welt"), geoml::Error);
}

// As the boost library has been removed after stripping down from TiGL, this test has been disabled since it uses the boost library.
TEST(OptionList, DISABLED_OptionNames)
{
    MockOptions options;
    ASSERT_EQ(2, options.GetNOptions());

    EXPECT_STREQ("my_double", options.GetOptionName(0).c_str());
    EXPECT_STREQ("my_string", options.GetOptionName(1).c_str());

    EXPECT_STREQ("Standard_Real", options.GetOptionType(0).c_str());
}

TEST(OptionList, HasOption)
{
    MockOptions options;

    EXPECT_TRUE(options.HasOption("my_string"));
    EXPECT_TRUE(options.HasOption("my_double"));

    EXPECT_FALSE(options.HasOption("my_new_string"));
}

TEST(OptionList, Merged)
{
    MockOptions first;
    MockOptionsOther second;
    geoml::COptionList options = first.Merged(second);

    EXPECT_TRUE(options.HasOption("my_string"));
    EXPECT_TRUE(options.HasOption("my_double"));
    EXPECT_TRUE(options.HasOption("my_int"));

    Standard_Real v1 = options.Get<double>("my_double");
    EXPECT_NEAR(0.0, v1.getValue(), 1e-10);

    int v2 = options.Get<int>("my_int");
    EXPECT_EQ(10, v2);

    EXPECT_FALSE(options.HasOption("my_new_string"));
}
