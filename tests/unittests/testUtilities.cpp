/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2012-11-21 Martin Siggel <Martin.Siggel@dlr.de>
*/
/**
* @file 
* @brief  Miscellaneous tests
*/

#include "test.h"
#include "common/CommonFunctions.h"
#include "logging/Logging.h"
#include "typename.h"

#include "BRepBuilderAPI_MakeWire.hxx"
#include "BRepBuilderAPI_MakeEdge.hxx"
#include "BRepAdaptor_CompCurve.hxx"
#include "GCPnts_AbscissaPoint.hxx"
#include "TopoDS_Wire.hxx"
#include "gp_Circ.hxx"

TEST(Misc, WireGetPoint)
{
    gp_Pnt p1(0.,0.,0.);
    gp_Pnt p2(1.,0.,0.);
    gp_Pnt p3(1.,3.,0.);
    
    BRepBuilderAPI_MakeWire wireBuilder;
    wireBuilder.Add(BRepBuilderAPI_MakeEdge(p1, p2));
    wireBuilder.Add(BRepBuilderAPI_MakeEdge(p2, p3));
    
    ASSERT_NEAR(4.0, GetLength(wireBuilder.Wire()), 1e-7);
    gp_Pnt p1_ = WireGetPoint(wireBuilder.Wire(), 0.0);
    gp_Pnt p2_ = WireGetPoint(wireBuilder.Wire(), 0.25);
    gp_Pnt p3_ = WireGetPoint(wireBuilder.Wire(), 1.0);
    
    ASSERT_NEAR(0.0, p1_.Distance(p1), 1e-7);
    ASSERT_NEAR(0.0, p2_.Distance(p2), 1e-7);
    ASSERT_NEAR(0.0, p3_.Distance(p3), 1e-7);
    
    p1_ = WireGetPoint(wireBuilder.Wire(), 0.0);
    p2_ = WireGetPoint(wireBuilder.Wire(), 0.25);
    p3_ = WireGetPoint(wireBuilder.Wire(), 1.0);
    
    ASSERT_NEAR(0.0, p1_.Distance(p1), 1e-7);
    ASSERT_NEAR(0.0, p2_.Distance(p2), 1e-7);
    ASSERT_NEAR(0.0, p3_.Distance(p3), 1e-7);
    
    ASSERT_NEAR(0.0 , ProjectPointOnWire(wireBuilder.Wire(), p1_), 1e-7);
    ASSERT_NEAR(0.25, ProjectPointOnWire(wireBuilder.Wire(), p2_), 1e-7);
    ASSERT_NEAR(1.0 , ProjectPointOnWire(wireBuilder.Wire(), p3_), 1e-7);
}

TEST(Misc, GetPointOnCirc)
{
    // add a half circle
    BRepBuilderAPI_MakeWire wireBuilder(BRepBuilderAPI_MakeEdge(gp_Circ(gp::XOY() ,1.), -M_PI/2, M_PI/2));
    // add a line
    wireBuilder.Add(BRepBuilderAPI_MakeEdge(gp_Pnt(0,1,0), gp_Pnt(-M_PI, 1, 0)));
    const TopoDS_Wire& wire = wireBuilder.Wire();
    
    
    gp_Pnt p1(0, -1, 0);
    gp_Pnt p2(1, 0, 0);
    gp_Pnt p3(0, 1, 0);
    gp_Pnt p4(-M_PI/2., 1, 0);
    gp_Pnt p5(-M_PI, 1, 0);
    
    ASSERT_NEAR(0.0, p1.Distance(WireGetPoint(wire,0.00)), 1e-7);
    ASSERT_NEAR(0.0, p2.Distance(WireGetPoint(wire,0.25)), 1e-7);
    ASSERT_NEAR(0.0, p3.Distance(WireGetPoint(wire,0.50)), 1e-7);
    ASSERT_NEAR(0.0, p4.Distance(WireGetPoint(wire,0.75)), 1e-7);
    ASSERT_NEAR(0.0, p5.Distance(WireGetPoint(wire,1.00)), 1e-7);

    ASSERT_NEAR(0.0, p1.Distance(WireGetPoint(wire,0.00)), 1e-7);
    ASSERT_NEAR(0.0, p2.Distance(WireGetPoint(wire,0.25)), 1e-7);
    ASSERT_NEAR(0.0, p3.Distance(WireGetPoint(wire,0.50)), 1e-7);
    ASSERT_NEAR(0.0, p4.Distance(WireGetPoint(wire,0.75)), 1e-7);
    ASSERT_NEAR(0.0, p5.Distance(WireGetPoint(wire,1.00)), 1e-7);
    
    ASSERT_NEAR(0.00, ProjectPointOnWire(wire, p1), 1e-7);
    ASSERT_NEAR(0.25, ProjectPointOnWire(wire, p2), 1e-7);
    ASSERT_NEAR(0.50, ProjectPointOnWire(wire, p3), 1e-7);
    ASSERT_NEAR(0.75, ProjectPointOnWire(wire, p4), 1e-7);
    ASSERT_NEAR(1.00, ProjectPointOnWire(wire, p5), 1e-7);
}


TEST(Misc, DISABLED_typeName)
{
    ASSERT_STREQ("geoml::Point", geoml::typeName(typeid(geoml::Point)).c_str());
    ASSERT_STREQ("geoml::Point", geoml::typeName<geoml::Point>().c_str());
}
