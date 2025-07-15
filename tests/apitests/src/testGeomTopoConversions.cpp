/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
*/

#include <geoml/geom_topo_conversions/geom_topo_conversions.h>
#include <geoml/geom_topo/modeling.h>
#include <geoml/curves/curves.h>

#include <gtest/gtest.h>

#include <Geom_BSplineCurve.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>

#include <vector>



TEST(Test_curveToEdge, simple_curve_to_edge_transformation)
{    

// define an open and clamped NURBS curve of degree 2

// control points
gp_Pnt p1(1.0, 0.0, 0.0);
gp_Pnt p2(1.0, 0.0, 1.0);
gp_Pnt p3(1.0, 0.0, 2.0);
gp_Pnt p4(1.0, 0.0, 3.0);
gp_Pnt p5(1.0, 0.0, 4.0);

std::vector<gp_Pnt> control_points {p1, p2, p3, p4, p5};

// degree:
Standard_Integer degree = 2;

// weights:
std::vector<Standard_Real> weights(5,1.0);

// knots: 
std::vector<Standard_Real> knots{0.0, 1.0, 2.0, 3.0};

// multiplicities: 
std::vector<int> mults {3, 1, 1, 3};

Handle(Geom_BSplineCurve) curve =
    geoml::nurbs_curve(
        control_points, 
        weights, 
        knots, 
        mults, 
        degree);

TopoDS_Edge edge = geoml::CurveToEdge(curve);


EXPECT_EQ(edge.ShapeType(), TopAbs_EDGE);


}

