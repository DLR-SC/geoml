/* 
* Copyright (C) 2016 German Aerospace Center (DLR/SC)
*
* Created: 2016-15-01 Martin Siggel <Martin.Siggel@dlr.de>
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

#include "ProjectOnLinearSpline.h"
#include "CPointsToLinearBSpline.h"
#include "geoml/error.h"

class TestLinearProjection : public ::testing::Test
{
protected:

    void SetUp() override
    {
        std::vector<gp_Pnt> points;
        points.push_back(gp_Pnt(0,0,0));
        points.push_back(gp_Pnt(2,0,0));
        points.push_back(gp_Pnt(2,1,0));
        points.push_back(gp_Pnt(1,1,0));

        curve = geoml::CPointsToLinearBSpline(points);
    }
    
    void TearDown() override
    {
    }
    
    Handle(Geom_BSplineCurve) curve;

};

TEST_F(TestLinearProjection, OnFirstSegment)
{
    geoml::ProjectOnLinearSpline proj(curve, gp_Pnt(1, 0, 0));
    ASSERT_TRUE(proj.IsDone());
    ASSERT_NEAR(0.25, proj.Parameter(), 1e-10);
    ASSERT_NEAR(0.0, gp_Pnt(1,0,0).Distance(proj.Point()), 1e-10);
}

TEST_F(TestLinearProjection, OnSecondSegment)
{
    geoml::ProjectOnLinearSpline proj(curve, gp_Pnt(2, 0.5, 0));
    ASSERT_TRUE(proj.IsDone());
    ASSERT_NEAR(0.625, proj.Parameter(), 1e-10);
    ASSERT_NEAR(0.0, gp_Pnt(2,0.5,0).Distance(proj.Point()), 1e-10);
}

TEST_F(TestLinearProjection, OnLastPoint)
{
    geoml::ProjectOnLinearSpline proj(curve, gp_Pnt(1, 1, 0));
    ASSERT_TRUE(proj.IsDone());
    ASSERT_NEAR(1.0, proj.Parameter(), 1e-10);
    ASSERT_NEAR(0.0, gp_Pnt(1,1,0).Distance(proj.Point()), 1e-10);
}

TEST_F(TestLinearProjection, InBetween)
{
    geoml::ProjectOnLinearSpline proj(curve, gp_Pnt(1, 0.5, 0));
    ASSERT_TRUE(proj.IsDone());
    ASSERT_NEAR(0.25, proj.Parameter(), 1e-10);
    ASSERT_NEAR(0.0, gp_Pnt(1,0,0).Distance(proj.Point()), 1e-10);
}

TEST_F(TestLinearProjection, InBetween2)
{
    geoml::ProjectOnLinearSpline proj(curve, gp_Pnt(1, 0.6, 0));
    ASSERT_TRUE(proj.IsDone());
    ASSERT_NEAR(1.0, proj.Parameter(), 1e-10);
    ASSERT_NEAR(0.0, gp_Pnt(1,1,0).Distance(proj.Point()), 1e-10);
}

TEST_F(TestLinearProjection, OutSide)
{
    geoml::ProjectOnLinearSpline proj(curve, gp_Pnt(-0.01, -0.01, 0));
    ASSERT_FALSE(proj.IsDone());
    
    ASSERT_THROW(proj.Point(), geoml::Error);
}
