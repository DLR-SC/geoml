/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2016 German Aerospace Center (DLR)
*
* Created: 2019-02-04 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "test.h"

#include "ProjectPointOnCurveAtAngle.h"
#include "geoml/error.h"
#include "common/CommonFunctions.h"

#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Geom_BSplineCurve.hxx>


class TestProjectionAtAngleSimple : public ::testing::Test
{
protected:

    void SetUp() override
    {
        TColStd_Array1OfReal knots(1,2);
        knots.SetValue(1, 0.);
        knots.SetValue(2, 1.0);
        
        TColStd_Array1OfInteger mults(1,2);
        mults.SetValue(1, 2);
        mults.SetValue(2, 2);
        
        TColgp_Array1OfPnt cps(1, 2);
        cps.SetValue(1, gp_Pnt(0., -1., 0.));
        cps.SetValue(2, gp_Pnt(0., 1., 0.));
        
        curve = new Geom_BSplineCurve(cps, knots, mults, 1);
        pnt = gp_Pnt(1., 0. ,0.);
    }
    
    Handle(Geom_BSplineCurve) curve;
    gp_Pnt pnt;

};

class TestProjectionAtAngleReference : public ::testing::Test
{
protected:

    void SetUp() override
    {
        TColStd_Array1OfReal knots(1,3);
        knots.SetValue(1, 0.);
        knots.SetValue(2, 0.5);
        knots.SetValue(3, 1.0);
        
        TColStd_Array1OfInteger mults(1,3);
        mults.SetValue(1, 4);
        mults.SetValue(2, 1);
        mults.SetValue(3, 4);
        
        TColgp_Array1OfPnt cps(1, 5);
        cps.SetValue(1, gp_Pnt(0., 0., 0.));
        cps.SetValue(2, gp_Pnt(1., 1., 0.));
        cps.SetValue(3, gp_Pnt(1., 2., 0.));
        cps.SetValue(4, gp_Pnt(-1., 3., 0.));
        cps.SetValue(5, gp_Pnt(-1., 4., 0.));
        
        curve = new Geom_BSplineCurve(cps, knots, mults, 3);
        pnt = gp_Pnt(4., 2. ,0);
    }
    
    Handle(Geom_BSplineCurve) curve;
    gp_Pnt pnt;

};

TEST_F(TestProjectionAtAngleSimple, consistency)
{
    for (double angle=45.; angle<=135; angle += 5) {
        double yExpected = 1./tan(Radians(angle));
        geoml::ProjectPointOnCurveAtAngle proj(pnt, curve, Radians(angle), gp_Dir(0, 0, 1));
        
        ASSERT_TRUE(proj.IsDone());
        EXPECT_EQ(1, proj.NbPoints());
        EXPECT_NEAR(0, proj.Point(1).Distance(gp_Pnt(0., yExpected, 0.)), 1e-8);
    }
}

TEST_F(TestProjectionAtAngleSimple, notPossible)
{
    geoml::ProjectPointOnCurveAtAngle proj(pnt, curve, Radians(30), gp_Dir(0, 0, 1));
    ASSERT_FALSE(proj.IsDone());
}

TEST_F(TestProjectionAtAngleReference,  degree150)
{
    geoml::ProjectPointOnCurveAtAngle proj(pnt, curve, Radians(150.), gp_Dir(0, 0, 1));

    ASSERT_TRUE(proj.IsDone());
    ASSERT_EQ(1, proj.NbPoints());
    ASSERT_NEAR(0.09729067568753146, proj.Parameter(1), 1e-8);
    ASSERT_NEAR(0., proj.Point(1).Distance(gp_Pnt(0.47384196, 0.53063481, 0.)), 1e-8);
}

TEST_F(TestProjectionAtAngleReference,  degree90)
{
    geoml::ProjectPointOnCurveAtAngle proj(pnt, curve, Radians(90.), gp_Dir(0, 0, 1));

    ASSERT_TRUE(proj.IsDone());
    ASSERT_EQ(1, proj.NbPoints());
    ASSERT_NEAR(0.32724554955847557, proj.Parameter(1), 1e-8);
    ASSERT_NEAR(0., proj.Point(1).Distance(gp_Pnt(0.81857595, 1.46111384, 0.)), 1e-8);
}

TEST_F(TestProjectionAtAngleReference,  degree20)
{
    geoml::ProjectPointOnCurveAtAngle proj(pnt, curve, Radians(20.), gp_Dir(0, 0, 1));

    ASSERT_FALSE(proj.IsDone());
    ASSERT_EQ(0, proj.NbPoints());
    EXPECT_THROW(proj.Parameter(1), geoml::Error);
}

TEST_F(TestProjectionAtAngleReference,  degree80Hard)
{
    geoml::ProjectPointOnCurveAtAngle proj(gp_Pnt(-4,2,0), curve, Radians(80.), gp_Dir(0, 0, 1));

    ASSERT_TRUE(proj.IsDone());
    ASSERT_EQ(2, proj.NbPoints());
    ASSERT_NEAR(0.30237394284100144, proj.Parameter(1), 1e-8);
    ASSERT_NEAR(0., proj.Point(1).Distance(gp_Pnt(0.82766784, 1.37624785, 0.)), 1e-8);

    ASSERT_NEAR(0.8548541018623607, proj.Parameter(2), 1e-8);
    ASSERT_NEAR(0., proj.Point(2).Distance(gp_Pnt(-0.78388606, 3.24329725, 0.)), 1e-8);
}
