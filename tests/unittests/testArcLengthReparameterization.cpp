/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2017 German Aerospace Center (DLR)
*
* Created: 2017-18-01 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "test.h"

#include "ArcLengthReparameterization.h"
#include <geoml/error.h>

#include <Geom_BSplineCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <GCPnts_AbscissaPoint.hxx>
#include <GeomAdaptor_Curve.hxx>

class TestArcLengthReparameterization : public ::testing::Test
{
protected:

    void SetUp() override
    {
        int degree = 2;

        TColgp_Array1OfPnt      cpoints(1, 4);
        TColStd_Array1OfReal    knots(1, 3);
        TColStd_Array1OfInteger mults(1, 3);

        cpoints.SetValue(1, gp_Pnt(0, 0, 0));
        cpoints.SetValue(2, gp_Pnt(1, 1, 0));
        cpoints.SetValue(3, gp_Pnt(2, 0, 0));
        cpoints.SetValue(4, gp_Pnt(3, 1, 0));

        knots.SetValue(1, 0.0);
        knots.SetValue(2, 0.2);
        knots.SetValue(3, 1.0);

        mults.SetValue(1, 3);
        mults.SetValue(2, 1);
        mults.SetValue(3, 3);

        curve = new Geom_BSplineCurve(cpoints, knots, mults, degree);
    }

    void TearDown() override
    {
    }

    Handle(Geom_BSplineCurve) curve;

};

TEST_F(TestArcLengthReparameterization, accuracy)
{
    geoml::ReparametrizeCurveByArclen repa;

    repa.init(curve, 1e-4);

    ASSERT_TRUE(repa.isInitialized());

    GeomAdaptor_Curve adaptor(curve);

    // total Length
    double totalLen = GCPnts_AbscissaPoint::Length(adaptor);

    EXPECT_NEAR(totalLen, repa.totalLength(), 1e-6);

    // check limits
    EXPECT_NEAR(0., repa.parameter(0.), 1e-4);
    EXPECT_NEAR(1., repa.parameter(totalLen), 1e-4);

    // at parameter 0.3, the the curve is roughly halved
    EXPECT_NEAR(0.3, repa.parameter(totalLen*0.5), 1e-2);

    for (double parm = 0.0; parm <= 1.0; parm += 0.05) {
        double arcLength = GCPnts_AbscissaPoint::Length(adaptor, 0.0, parm);

        double parmComputed = repa.parameter(arcLength);

        ASSERT_NEAR(parm, parmComputed, 1e-4);
    }

}

TEST_F(TestArcLengthReparameterization, notInititalized)
{
    geoml::ReparametrizeCurveByArclen repa;
    ASSERT_THROW(repa.parameter(0.), geoml::Error);
    
    ASSERT_THROW(repa.totalLength(), geoml::Error);
}
