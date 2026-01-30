/* 
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2012-11-21 Martin Siggel <Martin.Siggel@dlr.de>
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
    
    ASSERT_NEAR(4.0, GetLength(wireBuilder.Wire()).getValue(), 1e-7);
    gp_Pnt p1_ = WireGetPoint(wireBuilder.Wire(), 0.0);
    gp_Pnt p2_ = WireGetPoint(wireBuilder.Wire(), 0.25);
    gp_Pnt p3_ = WireGetPoint(wireBuilder.Wire(), 1.0);
    
    ASSERT_NEAR(0.0, p1_.Distance(p1).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p2_.Distance(p2).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p3_.Distance(p3).getValue(), 1e-7);
    
    p1_ = WireGetPoint(wireBuilder.Wire(), 0.0);
    p2_ = WireGetPoint(wireBuilder.Wire(), 0.25);
    p3_ = WireGetPoint(wireBuilder.Wire(), 1.0);
    
    ASSERT_NEAR(0.0, p1_.Distance(p1).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p2_.Distance(p2).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p3_.Distance(p3).getValue(), 1e-7);
    
    ASSERT_NEAR(0.0 , ProjectPointOnWire(wireBuilder.Wire(), p1_).getValue(), 1e-7);
    ASSERT_NEAR(0.25, ProjectPointOnWire(wireBuilder.Wire(), p2_).getValue(), 1e-7);
    ASSERT_NEAR(1.0 , ProjectPointOnWire(wireBuilder.Wire(), p3_).getValue(), 1e-7);
}

TEST(Misc, GetPointOnCirc)
{
    // add a half circle

    gp_Circ circ1 (gp::XOY() ,Standard_Real(1.));
    TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(circ1, -M_PI/2, M_PI/2).Edge();

    BRepBuilderAPI_MakeWire wireBuilder(edge1);
    // add a line

    TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(gp_Pnt(0,1,0), gp_Pnt(-M_PI, 1, 0)).Edge();

    wireBuilder.Add(edge2);
    const TopoDS_Wire& wire = wireBuilder.Wire();
    
    
    gp_Pnt p1(0, -1, 0);
    gp_Pnt p2(1, 0, 0);
    gp_Pnt p3(0, 1, 0);
    gp_Pnt p4(-M_PI/2., 1, 0);
    gp_Pnt p5(-M_PI, 1, 0);
    
    ASSERT_NEAR(0.0, p1.Distance(WireGetPoint(wire,0.00)).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p2.Distance(WireGetPoint(wire,0.25)).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p3.Distance(WireGetPoint(wire,0.50)).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p4.Distance(WireGetPoint(wire,0.75)).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p5.Distance(WireGetPoint(wire,1.00)).getValue(), 1e-7);

    ASSERT_NEAR(0.0, p1.Distance(WireGetPoint(wire,0.00)).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p2.Distance(WireGetPoint(wire,0.25)).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p3.Distance(WireGetPoint(wire,0.50)).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p4.Distance(WireGetPoint(wire,0.75)).getValue(), 1e-7);
    ASSERT_NEAR(0.0, p5.Distance(WireGetPoint(wire,1.00)).getValue(), 1e-7);
    
    ASSERT_NEAR(0.00, ProjectPointOnWire(wire, p1).getValue(), 1e-7);
    ASSERT_NEAR(0.25, ProjectPointOnWire(wire, p2).getValue(), 1e-7);
    ASSERT_NEAR(0.50, ProjectPointOnWire(wire, p3).getValue(), 1e-7);
    ASSERT_NEAR(0.75, ProjectPointOnWire(wire, p4).getValue(), 1e-7);
    ASSERT_NEAR(1.00, ProjectPointOnWire(wire, p5).getValue(), 1e-7);
}

// As the boost library has been removed after stripping down from TiGL, this test has been disabled since it uses the boost library.
TEST(Misc, DISABLED_typeName)
{
    ASSERT_STREQ("geoml::Point", geoml::typeName(typeid(geoml::Point)).c_str());
    ASSERT_STREQ("geoml::Point", geoml::typeName<geoml::Point>().c_str());
}
