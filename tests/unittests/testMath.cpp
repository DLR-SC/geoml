/* 
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2013-07-24 Martin Siggel <Martin.Siggel@dlr.de>
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
#include "math/MathFunctions.h"
#include <vector>

#include "geometry/Point.h"
#include "geometry/Transformation.h"

#include "math/Matrix.h"

#include <cstdlib>

TEST(Math, factorial)
{
    ASSERT_EQ(1, geoml::factorial(0));
    ASSERT_EQ(1, geoml::factorial(1));
    ASSERT_EQ(2, geoml::factorial(2));
    ASSERT_EQ(6, geoml::factorial(3));
    ASSERT_EQ(24, geoml::factorial(4));
}

TEST(Math, binom)
{
    ASSERT_EQ(1, geoml::binom(0,0));
    
    ASSERT_EQ(1, geoml::binom(3, 0));
    ASSERT_EQ(3, geoml::binom(3, 1));
    ASSERT_EQ(3, geoml::binom(3, 2));
    ASSERT_EQ(1, geoml::binom(3, 3));
    
    ASSERT_EQ(1,  geoml::binom(5, 0));
    ASSERT_EQ(5,  geoml::binom(5, 1));
    ASSERT_EQ(10, geoml::binom(5, 2));
    ASSERT_EQ(10, geoml::binom(5, 3));
    ASSERT_EQ(5,  geoml::binom(5, 4));
    ASSERT_EQ(1,  geoml::binom(5, 5));
    
    ASSERT_EQ(1,  geoml::binom(7, 0));
    ASSERT_EQ(7,  geoml::binom(7, 1));
    ASSERT_EQ(21, geoml::binom(7, 2));
    ASSERT_EQ(35, geoml::binom(7, 3));
    ASSERT_EQ(35, geoml::binom(7, 4));
    ASSERT_EQ(21, geoml::binom(7, 5));
    ASSERT_EQ(7,  geoml::binom(7, 6));
    ASSERT_EQ(1,  geoml::binom(7, 7));
    
    ASSERT_EQ(330, geoml::binom(11, 4));
}

TEST(Math, BernsteinPoly)
{
    ASSERT_NEAR(1.0, geoml::bernstein_poly(0,1,0.0).getValue(), 1e-7);
    ASSERT_NEAR(0.5, geoml::bernstein_poly(0,1,0.5).getValue(), 1e-7);
    ASSERT_NEAR(0.0, geoml::bernstein_poly(0,1,1.0).getValue(), 1e-7);
    
    ASSERT_NEAR(0.0, geoml::bernstein_poly(1,1,0.0).getValue(), 1e-7);
    ASSERT_NEAR(0.5, geoml::bernstein_poly(1,1,0.5).getValue(), 1e-7);
    ASSERT_NEAR(1.0, geoml::bernstein_poly(1,1,1.0).getValue(), 1e-7);
    
    ASSERT_NEAR(1.0, geoml::bernstein_poly(0,4,0.).getValue(), 1e-7);
    ASSERT_NEAR(0.0, geoml::bernstein_poly(0,4,1.).getValue(), 1e-7);
    
    ASSERT_NEAR(0.0, geoml::bernstein_poly(4,4,0.).getValue(), 1e-7);
    ASSERT_NEAR(1.0, geoml::bernstein_poly(4,4,1.).getValue(), 1e-7);
    
    ASSERT_NEAR(0.375, geoml::bernstein_poly(2,4,0.5).getValue(), 1e-7);
}

TEST(Math, QuadrilateralArea)
{
    // set square with side length 3
    geoml::Point A(0,0,0);
    geoml::Point B(3,0,0);
    geoml::Point C(3,3,0);
    geoml::Point D(0,3,0);
    ASSERT_NEAR(9., geoml::quadrilateral_area(A,B,C,D).getValue(), 1e-7);

    // rectangle
    C.y = 1.5;
    D.y = 1.5;
    ASSERT_NEAR(4.5, geoml::quadrilateral_area(A,B,C,D).getValue(), 1e-7);

    // trapezoid
    A = geoml::Point(0,0,0);
    B = geoml::Point(4,0,0);
    C = geoml::Point(3,1,0);
    D = geoml::Point(2,1,0);
    ASSERT_NEAR(2.5, geoml::quadrilateral_area(A,B,C,D).getValue(), 1e-7);

    // parallelogram
    A = geoml::Point(0,0,0);
    B = geoml::Point(4,0,0);
    C = geoml::Point(5,2,0);
    D = geoml::Point(1,2,0);
    ASSERT_NEAR(8., geoml::quadrilateral_area(A,B,C,D).getValue(), 1e-7);

    // triangle
    A = geoml::Point(0,0,0);
    B = geoml::Point(4,0,0);
    C = geoml::Point(2,2,0);
    D = geoml::Point(0,0,0);
    ASSERT_NEAR(4., geoml::quadrilateral_area(A,B,C,D).getValue(), 1e-7);
}

TEST(Math, CSTCurve)
{
    // Sample coefficients for shape function
    std::vector<Standard_Real> Br;
    Br.push_back(0.4);
    Br.push_back(1.0);
    Br.push_back(0.8);
    Br.push_back(10.2);
    
    // Constant sample coefficients for shape function
    std::vector<Standard_Real> B1(10,1.0);
    // Sample exponents for class function
    Standard_Real N1=0.1;
    Standard_Real N2=0.5;

    // check that shape function is constant for B constant
    ASSERT_NEAR(1.0, geoml::shape_function(B1, 0.0).getValue(), 1e-7);
    ASSERT_NEAR(1.0, geoml::shape_function(B1, 0.5).getValue(), 1e-7);
    ASSERT_NEAR(1.0, geoml::shape_function(B1, 1.0).getValue(), 1e-7);

    // check cst curve at some points
    ASSERT_NEAR(0.0, geoml::cstcurve(N1, N2, Br, 0.0, 0.0).getValue(), 1e-7);
    ASSERT_NEAR(0.568964089203402, geoml::cstcurve(N1, N2, Br, 0., 0.2).getValue(), 1e-7);
    ASSERT_NEAR(2.325867218509732, geoml::cstcurve(N1, N2, Br, 0., 0.75).getValue(), 1e-7);
    ASSERT_NEAR(0.0, geoml::cstcurve(N1, N2, Br, 0., 1.0).getValue(), 1e-7);

    // Sample exponents >1 for derivative 
    N1=1.1;
    N2=4.5;
    // check 1st derivative of cst curve at some points
    ASSERT_NEAR(0.0, geoml::cstcurve_deriv(N1, N2, Br, 0., 1, 0.0).getValue(), 1e-7);
    ASSERT_NEAR(0.0, geoml::cstcurve_deriv(N1, N2, Br, 0., 1, 1.0).getValue(), 1e-7);
    // check 1st derivative of cst curve at maximum of cstcurve (found numerically)
    ASSERT_NEAR(0.0, geoml::cstcurve_deriv(N1, N2, Br, 0., 1, 0.322954559162619).getValue(), 1e-7);
}

TEST(Math, Tchebycheff2Bezier_N3)
{
    math_Matrix Mmt = geoml::cheb_to_monomial(3);
    math_Matrix Mbm = geoml::monimial_to_bezier(3);
    
    // see Watkins 1988: degree reduction of bezier curves, p. 403
    
    math_Matrix M = Mbm * Mmt;
    M.Transpose();
    ASSERT_NEAR(1., M.Value(0,0).getValue(), 1e-12);
    ASSERT_NEAR(1., M.Value(0,1).getValue(), 1e-12);
    ASSERT_NEAR(1., M.Value(0,2).getValue(), 1e-12);
    
    ASSERT_NEAR(-1., M.Value(1,0).getValue(), 1e-12);
    ASSERT_NEAR( 0., M.Value(1,1).getValue(), 1e-12);
    ASSERT_NEAR( 1., M.Value(1,2).getValue(), 1e-12);
    
    ASSERT_NEAR( 1., M.Value(2,0).getValue(), 1e-12);
    ASSERT_NEAR(-3., M.Value(2,1).getValue(), 1e-12);
    ASSERT_NEAR( 1., M.Value(2,2).getValue(), 1e-12);
}

TEST(Math, Tchebycheff2Bezier_N4)
{
    math_Matrix Mmt = geoml::cheb_to_monomial(4);
    math_Matrix Mbm = geoml::monimial_to_bezier(4);
    
    // see Watkins 1988: degree reduction of bezier curves, p. 403
    
    math_Matrix M = Mbm * Mmt * 3;
    M.Transpose();
    ASSERT_NEAR(3., M.Value(0,0).getValue(), 1e-12);
    ASSERT_NEAR(3., M.Value(0,1).getValue(), 1e-12);
    ASSERT_NEAR(3., M.Value(0,2).getValue(), 1e-12);
    ASSERT_NEAR(3., M.Value(0,3).getValue(), 1e-12);
    
    ASSERT_NEAR(-3., M.Value(1,0).getValue(), 1e-12);
    ASSERT_NEAR(-1., M.Value(1,1).getValue(), 1e-12);
    ASSERT_NEAR(1.,  M.Value(1,2).getValue(), 1e-12);
    ASSERT_NEAR(3.,  M.Value(1,3).getValue(), 1e-12);
    
    ASSERT_NEAR(3.,  M.Value(2,0).getValue(), 1e-12);
    ASSERT_NEAR(-5., M.Value(2,1).getValue(), 1e-12);
    ASSERT_NEAR(-5., M.Value(2,2).getValue(), 1e-12);
    ASSERT_NEAR(3.,  M.Value(2,3).getValue(), 1e-12);
    
    ASSERT_NEAR(-3.,  M.Value(3,0).getValue(), 1e-12);
    ASSERT_NEAR(15.,  M.Value(3,1).getValue(), 1e-12);
    ASSERT_NEAR(-15., M.Value(3,2).getValue(), 1e-12);
    ASSERT_NEAR(3.,   M.Value(3,3).getValue(), 1e-12);
}

TEST(Math, Tchebycheff2Bezier_N5)
{
    math_Matrix Mmt = geoml::cheb_to_monomial(5);
    math_Matrix Mbm = geoml::monimial_to_bezier(5);
    
    // see Watkins 1988: degree reduction of bezier curves p. 403
    
    math_Matrix M = Mbm * Mmt * 6;
    M.Transpose();
    ASSERT_NEAR(6., M.Value(0,0).getValue(), 1e-12);
    ASSERT_NEAR(6., M.Value(0,1).getValue(), 1e-12);
    ASSERT_NEAR(6., M.Value(0,2).getValue(), 1e-12);
    ASSERT_NEAR(6., M.Value(0,3).getValue(), 1e-12);
    ASSERT_NEAR(6., M.Value(0,4).getValue(), 1e-12);
    
    ASSERT_NEAR(-6., M.Value(1,0).getValue(), 1e-12);
    ASSERT_NEAR(-3., M.Value(1,1).getValue(), 1e-12);
    ASSERT_NEAR(0.,  M.Value(1,2).getValue(), 1e-12);
    ASSERT_NEAR(3.,  M.Value(1,3).getValue(), 1e-12);
    ASSERT_NEAR(6.,  M.Value(1,4).getValue(), 1e-12);
    
    ASSERT_NEAR(6.,   M.Value(2,0).getValue(), 1e-12);
    ASSERT_NEAR(-6.,  M.Value(2,1).getValue(), 1e-12);
    ASSERT_NEAR(-10., M.Value(2,2).getValue(), 1e-12);
    ASSERT_NEAR(-6.,  M.Value(2,3).getValue(), 1e-12);
    ASSERT_NEAR(6,    M.Value(2,4).getValue(), 1e-12);
    
    ASSERT_NEAR(-6., M.Value(3,0).getValue(), 1e-12);
    ASSERT_NEAR(21., M.Value(3,1).getValue(), 1e-12);
    ASSERT_NEAR(0.,  M.Value(3,2).getValue(), 1e-12);
    ASSERT_NEAR(-21., M.Value(3,3).getValue(), 1e-12);
    ASSERT_NEAR(6.,   M.Value(3,4).getValue(), 1e-12);
    
    ASSERT_NEAR(6.,   M.Value(4,0).getValue(), 1e-12);
    ASSERT_NEAR(-42., M.Value(4,1).getValue(), 1e-12);
    ASSERT_NEAR(70.,  M.Value(4,2).getValue(), 1e-12);
    ASSERT_NEAR(-42., M.Value(4,3).getValue(), 1e-12);
    ASSERT_NEAR(6.,   M.Value(4,4).getValue(), 1e-12);
}

TEST(Math, Tchebycheff2Bezier_N6)
{
    math_Matrix Mmt = geoml::cheb_to_monomial(6);
    math_Matrix Mbm = geoml::monimial_to_bezier(6);
    
    // see Watkins 1988: degree reduction of bezier curves p. 404
    
    math_Matrix M = Mbm * Mmt * 5;
    M.Transpose();
    ASSERT_NEAR(5., M.Value(0,0).getValue(), 1e-12);
    ASSERT_NEAR(5., M.Value(0,1).getValue(), 1e-12);
    ASSERT_NEAR(5., M.Value(0,2).getValue(), 1e-12);
    ASSERT_NEAR(5., M.Value(0,3).getValue(), 1e-12);
    ASSERT_NEAR(5., M.Value(0,4).getValue(), 1e-12);
    ASSERT_NEAR(5., M.Value(0,5).getValue(), 1e-12);

    ASSERT_NEAR(-5., M.Value(1,0).getValue(), 1e-12);
    ASSERT_NEAR(-3., M.Value(1,1).getValue(), 1e-12);
    ASSERT_NEAR(-1., M.Value(1,2).getValue(), 1e-12);
    ASSERT_NEAR( 1., M.Value(1,3).getValue(), 1e-12);
    ASSERT_NEAR( 3., M.Value(1,4).getValue(), 1e-12);
    ASSERT_NEAR( 5., M.Value(1,5).getValue(), 1e-12);
    
    ASSERT_NEAR( 5., M.Value(2,0).getValue(), 1e-12);
    ASSERT_NEAR(-3., M.Value(2,1).getValue(), 1e-12);
    ASSERT_NEAR(-7., M.Value(2,2).getValue(), 1e-12);
    ASSERT_NEAR(-7., M.Value(2,3).getValue(), 1e-12);
    ASSERT_NEAR(-3., M.Value(2,4).getValue(), 1e-12);
    ASSERT_NEAR( 5., M.Value(2,5).getValue(), 1e-12);
    
    ASSERT_NEAR(-5., M.Value(3,0).getValue(), 1e-12);
    ASSERT_NEAR(13., M.Value(3,1).getValue(), 1e-12);
    ASSERT_NEAR( 7., M.Value(3,2).getValue(), 1e-12);
    ASSERT_NEAR(-7., M.Value(3,3).getValue(), 1e-12);
    ASSERT_NEAR(-13., M.Value(3,4).getValue(), 1e-12);
    ASSERT_NEAR( 5., M.Value(3,5).getValue(), 1e-12);
    
    ASSERT_NEAR(  5., M.Value(4,0).getValue(), 1e-12);
    ASSERT_NEAR(-27., M.Value(4,1).getValue(), 1e-12);
    ASSERT_NEAR( 21., M.Value(4,2).getValue(), 1e-12);
    ASSERT_NEAR( 21., M.Value(4,3).getValue(), 1e-12);
    ASSERT_NEAR(-27., M.Value(4,4).getValue(), 1e-12);
    ASSERT_NEAR(  5., M.Value(4,5).getValue(), 1e-12);
    
    ASSERT_NEAR(  -5., M.Value(5,0).getValue(), 1e-12);
    ASSERT_NEAR(  45., M.Value(5,1).getValue(), 1e-12);
    ASSERT_NEAR(-105., M.Value(5,2).getValue(), 1e-12);
    ASSERT_NEAR( 105., M.Value(5,3).getValue(), 1e-12);
    ASSERT_NEAR( -45., M.Value(5,4).getValue(), 1e-12);
    ASSERT_NEAR(   5., M.Value(5,5).getValue(), 1e-12);
}

TEST(Math, DistancePointFromLine)
{
    geoml::Point x0(0., 0., 0.);
    geoml::Point dx(1., 1., 0.);
    
    ASSERT_NEAR(0., geoml::distance_point_from_line(geoml::Point(0., 0., 0.), x0, dx).getValue(), 1e-10);
    ASSERT_NEAR(0., geoml::distance_point_from_line(geoml::Point(3., 3., 0.), x0, dx).getValue(), 1e-10);
    ASSERT_NEAR(sqrt(0.5), geoml::distance_point_from_line(geoml::Point(1., 0., 0.), x0, dx).getValue(), 1e-10);
}

TEST(Math, Transformation_Multiply)
{
    geoml::Transformation a;
    geoml::Transformation b;
    
    a.AddTranslation(2., 0., 0);
    
    b.AddScaling(0.4, 0.4, 0.4);
    b.AddTranslation(-0.1, 0.9, -0.3);
    
    geoml::Transformation c = a * b;
    EXPECT_NEAR(0.4, c.GetValue(0, 0).getValue(), 1e-10);
    EXPECT_NEAR(0.0, c.GetValue(0, 1).getValue(), 1e-10);
    EXPECT_NEAR(0.0, c.GetValue(0, 2).getValue(), 1e-10);
    EXPECT_NEAR(1.9, c.GetValue(0, 3).getValue(), 1e-10);
    
    EXPECT_NEAR(0.0, c.GetValue(1, 0).getValue(), 1e-10);
    EXPECT_NEAR(0.4, c.GetValue(1, 1).getValue(), 1e-10);
    EXPECT_NEAR(0.0, c.GetValue(1, 2).getValue(), 1e-10);
    EXPECT_NEAR(0.9, c.GetValue(1, 3).getValue(), 1e-10);
    
    EXPECT_NEAR(0.0, c.GetValue(2, 0).getValue(), 1e-10);
    EXPECT_NEAR(0.0, c.GetValue(2, 1).getValue(), 1e-10);
    EXPECT_NEAR(0.4, c.GetValue(2, 2).getValue(), 1e-10);
    EXPECT_NEAR(-0.3, c.GetValue(2, 3).getValue(), 1e-10);
    
    EXPECT_NEAR(0.0, c.GetValue(3, 0).getValue(), 1e-10);
    EXPECT_NEAR(0.0, c.GetValue(3, 1).getValue(), 1e-10);
    EXPECT_NEAR(0.0, c.GetValue(3, 2).getValue(), 1e-10);
    EXPECT_NEAR(1.0, c.GetValue(3, 3).getValue(), 1e-10);
}

TEST(Math, Transform_Decompose)
{
    // trivial test
    geoml::Transformation transformation;

    Standard_Real S[3] = {0., 0., 0.};
    Standard_Real R[3] = {0., 0., 0.};
    Standard_Real T[3] = {0., 0., 0.};
    transformation.Decompose(S, R, T);

    EXPECT_NEAR(S[0].getValue(), 1., 1e-8);
    EXPECT_NEAR(S[0].getValue(), 1., 1e-8);
    EXPECT_NEAR(S[0].getValue(), 1., 1e-8);
    EXPECT_NEAR(R[0].getValue(), 0., 1e-8);
    EXPECT_NEAR(R[0].getValue(), 0., 1e-8);
    EXPECT_NEAR(R[0].getValue(), 0., 1e-8);
    EXPECT_NEAR(T[0].getValue(), 0., 1e-8);
    EXPECT_NEAR(T[0].getValue(), 0., 1e-8);
    EXPECT_NEAR(T[0].getValue(), 0., 1e-8);
}

TEST(Math, Transform_Decompose2)
{
    // Simulate the case where a cpacs transformation as a rotation RX:0;RY:30;RZ:20
    // Remember that cpacs transformation has intrinsic rotation X,Y',Z'' so it corresponding to extrinsic rotation Z,Y,X
    // This above process is similar at the one used at CCPACSTransformation::updateMatrix
    geoml::Transformation rot;
    rot.AddRotationZ(20);
    rot.AddRotationY(30);
    rot.AddRotationX(0);

    // So now, as we can expected rotating the x basis vector (1,0,0) will output (0.81379768134, 0.34202014332 , -0.46984631039);
    gp_Pnt resultV = rot.Transform(gp_Pnt(1., 0. ,0.));
    gp_Pnt expectV = gp_Pnt(0.81379768134, 0.34202014332 , -0.46984631039);
    EXPECT_NEAR(resultV.X().getValue(), expectV.X().getValue(), 1e-8 );
    EXPECT_NEAR(resultV.Y().getValue(), expectV.Y().getValue(), 1e-8 );
    EXPECT_NEAR(resultV.Z().getValue(), expectV.Z().getValue(), 1e-8 );

    // but decomposing the rotation seems to output the X,Y,Z extrinsic angle
    Standard_Real S[3] = {0., 0., 0.};
    Standard_Real R[3] = {0., 0., 0.};
    Standard_Real T[3] = {0., 0., 0.};
    rot.Decompose(S, R, T);

    // so if we put back this value in transformation
    geoml::Transformation rot2;
    rot2.AddRotationZ(R[2]);
    rot2.AddRotationY(R[1]);
    rot2.AddRotationX(R[0]);

    // we do not get the expected result
    resultV = rot2.Transform(gp_Pnt(1., 0., 0.));
    EXPECT_NEAR(resultV.X().getValue(), expectV.X().getValue(), 1e-8 );
    EXPECT_NEAR(resultV.Y().getValue(), expectV.Y().getValue(), 1e-8 );
    EXPECT_NEAR(resultV.Z().getValue(), expectV.Z().getValue(), 1e-8 );
}

TEST(Math, SVD)
{
    geoml::Matrix A(1, 3, 1, 3);
    geoml::Matrix U(1, 3, 1, 3);
    geoml::Matrix S(1, 3, 1, 3);
    geoml::Matrix V(1, 3, 1, 3);
    geoml::Matrix USVt(1, 3, 1, 3);

    // trivial test
    A(1,1)=1.; A(1,2) = 0.; A(1,3) = 0.;
    A(2,1)=0.; A(2,2) = 1.; A(2,3) = 0.;
    A(3,1)=0.; A(3,2) = 0.; A(3,3) = 1.;

    geoml::SVD(A, U, S, V);
    USVt = U*S*V.Transposed();
    EXPECT_NEAR(USVt(1,1).getValue(), A(1,1).getValue(), 1e-8);
    EXPECT_NEAR(USVt(1,2).getValue(), A(1,2).getValue(), 1e-8);
    EXPECT_NEAR(USVt(1,3).getValue(), A(1,3).getValue(), 1e-8);
    EXPECT_NEAR(USVt(2,1).getValue(), A(2,1).getValue(), 1e-8);
    EXPECT_NEAR(USVt(2,2).getValue(), A(2,2).getValue(), 1e-8);
    EXPECT_NEAR(USVt(2,3).getValue(), A(2,3).getValue(), 1e-8);
    EXPECT_NEAR(USVt(3,1).getValue(), A(3,1).getValue(), 1e-8);
    EXPECT_NEAR(USVt(3,2).getValue(), A(3,2).getValue(), 1e-8);
    EXPECT_NEAR(USVt(3,3).getValue(), A(3,3).getValue(), 1e-8);

    // nontrivial test
    A(1,1)=1.; A(1,2) = 2.; A(1,3) = 3.;
    A(2,1)=4.; A(2,2) = 5.; A(2,3) = 6.;
    A(3,1)=7.; A(3,2) = 8.; A(3,3) = 9.;

    geoml::SVD(A, U, S, V);
    USVt = U*S*V.Transposed();
    EXPECT_NEAR(USVt(1,1).getValue(), A(1,1).getValue(), 1e-8);
    EXPECT_NEAR(USVt(1,2).getValue(), A(1,2).getValue(), 1e-8);
    EXPECT_NEAR(USVt(1,3).getValue(), A(1,3).getValue(), 1e-8);
    EXPECT_NEAR(USVt(2,1).getValue(), A(2,1).getValue(), 1e-8);
    EXPECT_NEAR(USVt(2,2).getValue(), A(2,2).getValue(), 1e-8);
    EXPECT_NEAR(USVt(2,3).getValue(), A(2,3).getValue(), 1e-8);
    EXPECT_NEAR(USVt(3,1).getValue(), A(3,1).getValue(), 1e-8);
    EXPECT_NEAR(USVt(3,2).getValue(), A(3,2).getValue(), 1e-8);
    EXPECT_NEAR(USVt(3,3).getValue(), A(3,3).getValue(), 1e-8);


}

TEST(Math, matrixIO)
{
    // This test checks matrix io routines by writing and reading a matrix
    // and comparing it to the reference values

    auto randfloat = [](double min, double max) {
        double random = ((double) std::rand()) / static_cast<double>(RAND_MAX);
        double range = max - min;
        return (random*range) + min;
    };
    
    geoml::Matrix matRef(1, 5, 2, 7);
    // some random initialization
    for (int i = matRef.LowerRow(); i <= matRef.UpperRow(); ++i) {
        for (int j = matRef.LowerCol(); j <= matRef.UpperCol(); ++j) {
            matRef.Value(i, j) = randfloat(1.0, 2.0);
        }
    }

    std::string filename = "tmpMatrix.mat";

    geoml::writeMatrix(matRef, filename);
    auto mat = geoml::readMatrix(filename);

    //std::cout << "reading done" << mat.Value(1,1).getValue() << std::endl; 

    ASSERT_EQ(mat.LowerCol(), matRef.LowerCol());
    ASSERT_EQ(mat.UpperCol(), matRef.UpperCol());
    ASSERT_EQ(mat.LowerRow(), matRef.LowerRow());
    ASSERT_EQ(mat.UpperRow(), matRef.UpperRow());

    for (int i = matRef.LowerRow(); i <= matRef.UpperRow(); ++i) {
        for (int j = matRef.LowerCol(); j <= matRef.UpperCol(); ++j) {
            EXPECT_EQ(matRef.Value(i, j).getValue(), mat.Value(i, j).getValue());
        }
    }
}
