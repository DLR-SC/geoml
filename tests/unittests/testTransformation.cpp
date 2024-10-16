/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2020 German Aerospace Center (DLR)
*/

#include "test.h"

#include "geometry/PointsToBSplineInterpolation.h"
#include "geometry/CurvesToSurface.h"
#include "common/CommonFunctions.h"
#include "geometry/Transformation.h"

TEST(Transformation, transformSurface)
{
    auto pnts1 = OccArray({
        gp_Pnt(0., 0., 0.),
        gp_Pnt(1., 0, 0.),
    });

    auto pnts2 = OccArray({
        gp_Pnt(0., 1., 0.),
        gp_Pnt(1., 1.0, 0.),
    });

    auto c1   = geoml::PointsToBSplineInterpolation(pnts1).Curve();
    auto c2   = geoml::PointsToBSplineInterpolation(pnts2).Curve();
    auto surf = geoml::CurvesToSurface({c1, c2}).Surface();

    EXPECT_EQ(0.0, surf->Value(0., 0.).Distance(gp_Pnt(0., 0., 0.)));
    EXPECT_EQ(0.0, surf->Value(1., 1.).Distance(gp_Pnt(1., 1., 0.)));
    
    geoml::Transformation transl;
    transl.AddTranslation(0, 0, 1.);

    auto surfTrans = transl.Transform(surf);

    EXPECT_NEAR(0.0, surfTrans->Value(0., 0.).Distance(gp_Pnt(0., 0., 1.)), 1e-12);
    EXPECT_NEAR(0.0, surfTrans->Value(1., 1.).Distance(gp_Pnt(1., 1., 1.)), 1e-12);
    
    geoml::Transformation rot;
    rot.AddRotationZ(90.);

    auto surfRot = rot.Transform(surf);
    EXPECT_NEAR(0.0, surfRot->Value(0., 0.).Distance(gp_Pnt(0., 0., 0.)), 1e-12);
    EXPECT_NEAR(0.0, surfRot->Value(1., 1.).Distance(gp_Pnt(-1., 1., 0.)), 1e-12);
    
    
    geoml::Transformation scale;
    scale.AddScaling(2, 0.5, 1.);

    auto surfScale = scale.Transform(surf);
    EXPECT_NEAR(0.0, surfScale->Value(0., 0.).Distance(gp_Pnt(0., 0., 0.)), 1e-12);
    EXPECT_NEAR(0.0, surfScale->Value(1., 1.).Distance(gp_Pnt(2., 0.5, 0.)), 1e-12);
}
