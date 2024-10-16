/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2020 German Aerospace Center (DLR)
*
* Created: 2020 Jan Kleinert <Jan.Kleinert@dlr.de>
*/

#include "test.h"
#include "PatchShellTool.h"
#include "geoml/error.h"
#include "geometry/PointsToBSplineInterpolation.h"
#include "geometry/CurvesToSurface.h"

#include "BRepBuilderAPI_MakeWire.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRepBuilderAPI_MakeFace.hxx"
#include "BRep_Builder.hxx"


TEST(PatchShellTool, Success)
{
    // FIRST, BUILD A SHAPE

    std::vector<Handle(Geom_Curve)> curves;

    // create points for first curve
    Handle(TColgp_HArray1OfPnt) pnt1 = new TColgp_HArray1OfPnt(1, 5);
    pnt1->SetValue(1, gp_Pnt(1., 0., 0.5));
    pnt1->SetValue(2, gp_Pnt(0., 1., -0.75));
    pnt1->SetValue(3, gp_Pnt(-1., 0., 0.25));
    pnt1->SetValue(4, gp_Pnt(0., -1., 0.));
    pnt1->SetValue(5, gp_Pnt(1., 0., -0.1));

    // interpoate points to (planar) curve
    geoml::PointsToBSplineInterpolation app1(pnt1, 3, true);
    Handle(Geom_BSplineCurve) base_curve = app1.Curve();
    curves.push_back(base_curve);

    // create points for second curve
    Handle(TColgp_HArray1OfPnt) pnt2 = new TColgp_HArray1OfPnt(1, 5);
    pnt2->SetValue(1, gp_Pnt(1., 0., 5.));
    pnt2->SetValue(2, gp_Pnt(0., 1., 5.));
    pnt2->SetValue(3, gp_Pnt(-1., 0., 5.));
    pnt2->SetValue(4, gp_Pnt(0., -1., 5.));
    pnt2->SetValue(5, gp_Pnt(1., 0., 5.));

    // interpoate points to curve
    geoml::PointsToBSplineInterpolation app2(pnt2, 3, true);
    Handle(Geom_BSplineCurve) tip_curve = app2.Curve();
    gp_Trsf T;
    T.SetScale(gp_Pnt(0., 0., 5.), 0.25);
    tip_curve->Transform(T);
    curves.push_back(tip_curve);

    // interpolate wires
    geoml::CurvesToSurface c2s(curves);
    TopoDS_Shape myShape = BRepBuilderAPI_MakeFace(c2s.Surface(), 1e-6).Face();


    // NOW PERFORM SOME TESTS
    
    geoml::PatchShellTool patcher(myShape, 1e-6);
    EXPECT_NO_THROW(TopoDS_Shape result = patcher.PatchedShape(););
}

TEST(PatchShellTool, emptyShape)
{
    TopoDS_Shape emptyShape;
    geoml::PatchShellTool patcher(emptyShape, 1e-6);
    EXPECT_THROW(patcher.PatchedShape();, geoml::Error);
}

TEST(PatchShellTool, brokenShape)
{
    BRep_Builder b;
    TopoDS_Compound brokenShape;
    b.MakeCompound(brokenShape);
    b.Add(brokenShape, BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(0., 0., 0.1)).Edge());
    b.Add(brokenShape, BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(0., 0., -0.1)).Edge());
    geoml::PatchShellTool patcher(brokenShape, 1e-6);
    EXPECT_THROW(patcher.PatchedShape();, geoml::Error);
}
