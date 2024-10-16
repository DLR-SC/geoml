/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2020 German Aerospace Center (DLR)
*/

#include "test.h"

#include <TopoAlgorithms.h>
#include <geometry/Transformation.h>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <gp_Pnt.hxx>

TEST(TopoAlgorithms, IsDegenerated)
{
    TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(gp_Pnt(0., 0., 0.), gp_Pnt(1., 0., 0.)).Edge();
    TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(gp_Pnt(1., 0., 0.), gp_Pnt(1., 1., 0.)).Edge();

    TopoDS_Wire wire = BRepBuilderAPI_MakeWire(e1, e2).Wire();
    
    EXPECT_FALSE(geoml::TopoAlgorithms::IsDegenerated(wire));
    
    geoml::Transformation trafo;
    trafo.AddScaling(0., 0., 0.);

    wire = TopoDS::Wire(trafo.Transform(wire));
    EXPECT_TRUE(geoml::TopoAlgorithms::IsDegenerated(wire));
}
