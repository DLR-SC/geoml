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


#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include "geoml_internal.h"
#include <vector>
#include <math_Vector.hxx>
#include <Precision.hxx>
#include "Matrix.h"

namespace geoml 
{

class Point;

/**
 * @brief Computes the binomial coefficient (n,k)
 */
GEOML_EXPORT int binom(int n, int k);

/**
 * @brief Computes the factorial n! in a non recursive fashion
 */
GEOML_EXPORT int factorial(int n);

/**
 * @brief Computes the power x^n
 * @param x Basis
 * @param n Exponent 
 * @return x^n
 */
GEOML_EXPORT Standard_Real pow_int(Standard_Real x, int n);

/** 
 * @brief Computes the values of the i-th bernstein polynome 
 * with degree n at position x
 *
 *@param i Number of the polynome with 0 <= i <= n
 *@param n Degree of the polynome, n > 0
 *@param x Value the polynom should be evaluated at (normally 0 <= x <= 1, but not necessarily)
*/
GEOML_EXPORT Standard_Real bernstein_poly(int i, int n, Standard_Real x);

/** 
 * @brief Computes the values of the k-th derivative of the 
 * i-th bernstein polynome with degree n at position x
 *
 *@param k Order of derivative
 *@param i Number of the polynome with 0 <= i <= n
 *@param n Degree of the polynome, n > 0
 *@param x Value the polynom should be evaluated at (normally 0 <= x <= 1, but not necessarily)
*/
GEOML_EXPORT Standard_Real bernstein_poly_deriv(int k, int i, int n, Standard_Real x);

/**
 * @brief Calculated the area of a quadrilateral defined by the 4 corner points A,B,C,D
 */
GEOML_EXPORT Standard_Real quadrilateral_area(const Point& A, const Point& B, const Point& C, const Point& D);

/**
 * @brief Calculates the distance of a point P from the line defined by a point X0 and direction DX
 */
GEOML_EXPORT Standard_Real distance_point_from_line(const Point& P, const Point& X0, const Point& DX);

/**
 * @brief Computes the nth derivative of x^k
 */
GEOML_EXPORT Standard_Real pow_deriv(Standard_Real x, Standard_Real k, int n);

/** @brief defines the class function
 * C(psi) = psi^N1 * (1-psi)^N2
 * for a CST air profile curve
 */
GEOML_EXPORT Standard_Real class_function(const Standard_Real& N1, const Standard_Real& N2, const Standard_Real& x);

/** @brief defines the derivative of the class function
 * C(psi) = psi^N1 * (1-psi)^N2
 * for a CST air profile curve
 */
GEOML_EXPORT Standard_Real class_function_deriv(const Standard_Real& N1, const Standard_Real& N2, const int& n, const Standard_Real& x);

/** @brief defines the shape function
 * S(psi)=sum_i=1^N B_i * P_i^n(psi)
 * for a CST air profile curve. The P_i^n are the Bernstein polynomials.
 */
GEOML_EXPORT Standard_Real shape_function(const std::vector<Standard_Real>& B, const Standard_Real& x);

/** @brief defines the derivative of the shape function
 * S(psi)=sum_i=1^N B_i * P_i^n(psi)
 * for a CST air profile curve. The P_i^n are the Bernstein polynomials.
 */
GEOML_EXPORT Standard_Real shape_function_deriv(const std::vector<Standard_Real>& B, const int& n, const Standard_Real& x);

/** @brief defines the CST air profile curve according to
 * "CST" Universal Paramteric Geometry Representation Method
 * with Applications to Supersonic Aircraft, B.M. Kulfan 2007
 * 
 * CST(psi)=C(psi)*S(psi)
 *
 * N1, N2 are the parameters of the class function C(psi) = psi^N1 * (1-psi)^N2
 * B is the vector of coefficients for the bernstein polynomials P_i^n(psi) 
 * T is the trailing edge thickness
 * inside the shape function S(psi)=sum_i=1^N B_i * p_i^n(psi)
 * The order of the Bernstein polynomials N is defined by the length of the B vector
 */
GEOML_EXPORT Standard_Real cstcurve(const Standard_Real& N1, const Standard_Real& N2, const std::vector<Standard_Real>& B, const Standard_Real& T, const Standard_Real& x);

/** @brief defines the derivative of the CST air profile curve 
 * CST(psi)=C(psi)*S(psi)
 */
GEOML_EXPORT Standard_Real cstcurve_deriv(const Standard_Real& N1, const Standard_Real& N2, const std::vector<Standard_Real>& B, const Standard_Real& T, const int& n, const Standard_Real& x);

/**
 * Return true if the value of a is similar to b
 * @param a
 * @param b
 * @param epsilon
 * @return
 */
GEOML_EXPORT bool isNear(Standard_Real a, Standard_Real b, Standard_Real epsilon = Precision::Confusion());

/**
 * 1D Function interface accepting one parameter t and returning
 * the function value
 */
class MathFunc1d
{
public:
    GEOML_EXPORT virtual ~MathFunc1d(){}
    virtual Standard_Real value(Standard_Real t) = 0;
};

/**
 * 3D Function interface accepting one parameter t and returning
 * the function value
 */
class MathFunc3d
{
public:
    GEOML_EXPORT virtual ~MathFunc3d(){}
    virtual Standard_Real valueX(Standard_Real t) = 0;
    virtual Standard_Real valueY(Standard_Real t) = 0;
    virtual Standard_Real valueZ(Standard_Real t) = 0;
};

/**
 * @brief Chebycheff approximation of a function f(x), x in [a,b]
 *
 * @throws Error (geoml::MATH_ERROR) if N <= 0 
 * 
 * @param func The 1D function to approximate
 * @param N Number of function evaluations ( = polynomial degree + 1)
 * @param a First parameter of the function to approximate
 * @param b Last  parameter of the function to approximate
 * @return Chebycheff coefficients
 */
GEOML_EXPORT math_Vector cheb_approx(MathFunc1d& func, int N, Standard_Real a, Standard_Real b);


/** 
 * @brief Computes the chebycheff to monomial transformation matrix
 * 
 * Use this NxN matrix to translate chebychev coefficients into monomial
 * coefficients of polynomes.
 * 
 * @param N Dimension of the matrix
 */
GEOML_EXPORT math_Matrix cheb_to_monomial(int N);

/**
* @brief Monomial to Bernstein conversion matrix
*
 * Use this NxN matrix to translate monimial coefficients into bezier
 * coefficients (control points).
*/
GEOML_EXPORT math_Matrix monimial_to_bezier(int N);

/**
* @brief  A function that calculates the polar decomposition of a 3x3 matrix A.
*
* PolarDecomposition(A,U,P) calculates the polar decomposition U,P of the input
* matrix A, such that A = U x P, where U is a unitary matrix and P is a positive
* semi-definite Hermitian matrix. U can be interpreted as a rotation and P as a
* spatial scaling, possibly including off-diaogonal shearing terms
*
* @param A input matrix
* @param U unitary matrix U
* @param P hermitian matrix P
*/
GEOML_EXPORT void PolarDecomposition(Matrix const& A, Matrix& U, Matrix& P);

GEOML_EXPORT void SVD(Matrix const& A, Matrix& U, Matrix& S, Matrix& V);

/**
 * Linear interpolation in of xdata<->ydata array at position x
 */
GEOML_EXPORT Standard_Real Interpolate(const std::vector<Standard_Real>& xdata, const std::vector<Standard_Real>& ydata, Standard_Real x);

} // namespace geoml

#endif // MATHFUNCTIONS_H
