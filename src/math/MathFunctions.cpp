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

#include "MathFunctions.h"
#include "geoml/error.h"

#include <cassert>
#include <algorithm>
#include <cmath>
#include <vector>

#include <geometry/Point.h>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>

#include "Matrix.h"

#include <math_Recipes.hxx>
#include <logging/Logging.h>

namespace geoml 
{

/**
 * @brief Computes the binomial coefficient (n,k)
 */
int binom(int n, int k) 
{
    // this is an efficient implementation, taken from wikipedia
    if (k == 0) {
        return 1;
    }

    if (2*k > n) {
        return binom(n, n-k);
    }
    else {
        int result = n-k+1;
        for (int i = 2; i <= k; ++i){
            result *= (n - k + i);
            result /= i;
        }
        return result;
    }
}

/**
 * @brief Computes the factorial n! in a non recursive fashion
 */
int factorial(int n)
{
    assert(n >= 0);
    
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    
    return result;
}

/**
 * @brief Computes the power x^n
 */
Standard_Real pow_int(Standard_Real x, int n) 
{
    assert(n >= 0);
    
    Standard_Real result = 1.;
    for (int i = 0; i < n; ++i) {
        result *= x;
    }
    
    return result;
}

/** 
 * @brief Computes the values of the i-th bernstein polynome 
 * with degree n at position x
*/
Standard_Real bernstein_poly(int i, int n, Standard_Real x) 
{
    assert(i <= n);
    return (Standard_Real)binom(n,i) * pow_int(x, i) * pow_int(1.-x, n-i);
}

/** 
 * @brief Computes the values of the k-th derivative of the 
 * i-th bernstein polynome with degree n at position x
 *
 * Equation taken from: 
 * Doha et al (2011): On the Derivatives of Bernstein Polynomials: An Application for
 * the Solution of High Even-Order Differential Equations
*/
Standard_Real bernstein_poly_deriv(int k, int i, int n, Standard_Real x) 
{
    if (k <= 0) {
        return bernstein_poly(i, n, x);
    }

    // The k-th derivative of a polynom at degree n < k is always zero
    if (k > n) {
        return 0.;
    }
    
    int jmin = std::max(0, i + k - n);
    int jmax = std::min(i, k);
    
    Standard_Real result = 0.;
    for (int j = jmin; j <= jmax; ++j) {
        result += pow_int(-1., j + k) * binom(k, j) * bernstein_poly(i - j, n - k, x);
    }
    result *= factorial(n)/factorial(n-k);
    return result;
}

/**
 * @brief Calculated the area of a quadrilateral defined by the 4 corner points A,B,C,D
 */
Standard_Real quadrilateral_area(const Point& A, const Point& B, const Point& C, const Point& D) 
{
    gp_Vec AC(A.Get_gp_Pnt(),C.Get_gp_Pnt());
    gp_Vec BD(B.Get_gp_Pnt(),D.Get_gp_Pnt());

    return 0.5 * AC.CrossMagnitude(BD);
}

/**
 * @brief Calculates the distance of a point P from the line defined by a point X0 and direction DX
 */
Standard_Real distance_point_from_line(const Point& P, const Point& X0, const Point& DX)
{
    Standard_Real lenDX = DX.norm2();
    assert(lenDX > 0.);
    
    return Point::cross_prod(DX, X0-P).norm2()/lenDX;
}

/** 
 * @brief Computes the nth derivative of x^k
 */
Standard_Real pow_deriv(Standard_Real x, Standard_Real k, int n) 
{
    assert(n >= 0);

    if (n == 0) {
        return pow(x,k);
    }

    Standard_Real eps = 1e-15;

    // check if k is a natural number. if yes, we have to return 0 if n > k
    if ( fabs(k - int(k)) < eps && n > k) {
        return 0.;
    }
    else {
        Standard_Real fact = 1.;
        for (int i = 0; i < n; ++i){
            fact *= (k-i);
        }
        return fact * pow(x, k - (Standard_Real)n);
    }
}

/** @brief defines the class function
 * C(psi) = psi^N1 * (1-psi)^N2
 * for a CST air profile curve
 */
Standard_Real class_function(const Standard_Real& N1, const Standard_Real& N2, const Standard_Real& x)
{
    return pow(x,N1) * pow(1-x,N2);
}
/** @brief defines the derivative of the class function
 * C(psi) = psi^N1 * (1-psi)^N2
 * for a CST air profile curve
 */
Standard_Real class_function_deriv(const Standard_Real& N1, const Standard_Real& N2, const int& n, const Standard_Real& x)
{
    Standard_Real res = 0.;
    for (int i = 0; i <= n; i++) {
        res += geoml::binom(n,i)
             * geoml::pow_deriv(x,N1,i)
             * geoml::pow_deriv(1-x, N2, n-i)
             * geoml::pow_int(-1., n-i);
    }
    return res;
}
/** @brief defines the shape function
 * S(psi)=sum_i=1^N B_i * P_i^n(psi)
 * for a CST air profile curve. The P_i^n are the Bernstein polynomials.
 */
Standard_Real shape_function(const std::vector<Standard_Real>& B, const Standard_Real& x)
{
    Standard_Real ret = 0.;
    int order = static_cast<int>(B.size()) - 1;
    int i = 0;
    for (std::vector<Standard_Real>::const_iterator bIT = B.begin(); bIT != B.end(); ++bIT, ++i) {
        ret += *bIT * bernstein_poly(i, order, x);
    }
    return ret;
}
/** @brief defines the derivative of the shape function
 * S(psi)=sum_i=1^N B_i * P_i^n(psi)
 * for a CST air profile curve. The P_i^n are the Bernstein polynomials.
 */
Standard_Real shape_function_deriv(const std::vector<Standard_Real>& B, const int& n, const Standard_Real& x)
{
    Standard_Real ret = 0.;
    int order = static_cast<int>(B.size()) - 1;
    int i = 0;
    for (std::vector<Standard_Real>::const_iterator bIT = B.begin(); bIT != B.end(); ++bIT, ++i) {
        ret += *bIT * geoml::bernstein_poly_deriv(n, i, order, x);
    }
    return ret;
}
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
Standard_Real cstcurve(const Standard_Real& N1, const Standard_Real& N2, const std::vector<Standard_Real>& B, const Standard_Real& T, const Standard_Real& x)
{
        return class_function(N1, N2, x) * shape_function(B, x) + x*T;
}
/** @brief defines the derivative of the CST air profile curve 
 * CST(psi)=C(psi)*S(psi)
 */
Standard_Real cstcurve_deriv(const Standard_Real& N1, const Standard_Real& N2, const std::vector<Standard_Real>& B, const Standard_Real& T, const int& n, const Standard_Real& x)
{
    Standard_Real res = 0.;
    for (int i= 0; i<= n; i++) {
        res += geoml::binom(n,i)
             * class_function_deriv(N1, N2, i, x)
             * shape_function_deriv(B, n-i, x);
    }
    if (n == 1) {
        res += T;
    }
    
    return res;
}

/**
 * @brief Chebycheff approximation of a function f(x), x in [a,b]
 *
 * @param func The 1D funktion to approximate
 * @param parms Additional data passed to the function func (may be NULL)
 * @param N Number of function evaluations ( = polynomial degree + 1)
 * @param a First parameter of the function to approximate
 * @param b Last  parameter of the function to approximate
 * @return Chebycheff coefficients
 */
math_Vector cheb_approx(MathFunc1d& func, int N, Standard_Real a, Standard_Real b)
{
    if (N <= 0) {
        throw Error("N <= 0 in cheb_approx", geoml::MATH_ERROR);
    }
    math_Vector c(0, N-1);
    
    Standard_Real * fx = new Standard_Real[N];
    for (int k = 1; k <= N; ++k) {
        Standard_Real x = cos(M_PI * ((Standard_Real)k - 0.5) / (Standard_Real)N);
        // shift to correct intervall
        x = (x + 1.) / 2. *(b-a) + a;
        
        // evaluate function at x
        fx[k-1] = func.value(x);
    }
    
    for (int j = 0; j < N; ++j) {
        Standard_Real cj = 0;
        for (int k = 1; k <= N; ++k) {
            Standard_Real v = fx[k-1] * cos(M_PI * (Standard_Real)j * ((Standard_Real)k - 0.5)/ (Standard_Real)N);
            cj += 2./(Standard_Real) N * v;
        }
        c(j) = cj;
    }
    delete[] fx;
    
    c(0) *= 0.5;
    
    return c;
}

/// computes the chebycheff to monomial transformation matrix
math_Matrix cheb_to_monomial(int N)
{
    math_Matrix matrix(0,N-1, 0, N-1, 0.);
    
    matrix(0,0) =1.;
    matrix(0,1) =-1.;
    matrix(1,1) =2.;
    
    for (int j = 2; j < N; ++j) {
        for (int i = 0; i <= j; ++i) {
            Standard_Real val = 0.;
            if (i > 0) {
                val += 4.*matrix(i-1, j-1);
            }
            val -= 2.*matrix(i,j-1) + matrix(i,j-2);
            
            matrix(i,j)= val;
        }
    }
    return matrix;
}

/**
* @brief Monomial to Bernstein conversion matrix
*
* Returns a NxN matrix to be multiplied with
* a vector of Monomial coefficients,
* generating a vector of Bernstein coefficients.
*/
math_Matrix monimial_to_bezier(int N)
{
    math_Matrix matrix(0,N-1, 0, N-1, 0.);
    
    for (int j=0; j < N; ++j) {
        for (int i = j; i < N; ++i) {
            Standard_Real div1= binom(i, i-j);
            Standard_Real div2= binom(N-1, j);
            matrix(i,j) = div1 / div2;
        }
    }
    
    return matrix;
}

void PolarDecomposition(Matrix const&A, Matrix& U, Matrix& P)
{
    // calculate SVD
    Matrix u(1, 3, 1, 3);
    Matrix s(1, 3, 1, 3);
    Matrix v(1, 3, 1, 3);
    SVD(A, u, s, v);

    // U = uv*
    U = u*v.Transposed();

    // P = vsv*
    P = v*s*v.Transposed();
}

void SVD(Matrix const& A, Matrix& U, Matrix& S, Matrix& V)
{

    Vector Sv(1,3);
    U = A;

    SVD_Decompose(U, Sv, V);

    S(1,1) = Sv(1);
    S(1,2) = 0.;
    S(1,3) = 0.;

    S(2,1) = 0.;
    S(2,2) = Sv(2);
    S(2,3) = 0.;

    S(3,1) = 0.;
    S(3,2) = 0.;
    S(3,3) = Sv(3);
}

/// searches for i, such that xdata[i] <= x < xdata[i+1]
/// used by linear interpolation function
size_t FindPosition(const std::vector<Standard_Real>& xdata, Standard_Real x)
{
    // we assume, that the xvalues are ordered in ascending order
    size_t ilow = 0;
    size_t ihigh = xdata.size()-1;

    assert(xdata.size() >= 2);

    // check extrapolation cases
    if (x < xdata[ilow]) {
        return ilow;
    }

    if (x >= xdata[ihigh]) {
        return ihigh - 1;
    }

    // now do the search
    while (ilow < ihigh - 1) {
        size_t imid = (ilow + ihigh)/2;
        if (xdata[ilow]<= x && x < xdata[imid]) {
            ihigh = imid;
        }
        else if(xdata[imid] <= x && x < xdata[ihigh]) {
            ilow = imid;
        }
        else {
            // this case can only occur, if
            // input data are not ordered
            return xdata.size();
        }
    }

    // we found the value
    assert(xdata[ilow] <= x && x < xdata[ilow+1]);
    return ilow;
}

/// linear interpolation in of xdata<->ydata array at position x
Standard_Real Interpolate(const std::vector<Standard_Real>& xdata, const std::vector<Standard_Real>& ydata, Standard_Real x)
{
    if (xdata.size() == 0) {
        return 0.;
    }

    if (xdata.size() == 1) {
        return ydata[0];
    }

    if (x < xdata[0] || x > xdata[xdata.size() -1]) {
        // extrapolation
        LOG(WARNING) << "Extrapolating at x=" << x << ". XData is in range "
                     << xdata[0] << "..." << xdata[xdata.size() -1] << ".";
    }

    size_t pos = FindPosition(xdata, x);

    assert(pos < (unsigned int)(xdata.size() - 1));
    assert(ydata.size() == xdata.size());

    Standard_Real y = (ydata[pos+1] - ydata[pos])/(xdata[pos+1]-xdata[pos]) * (x - xdata[pos]) + ydata[pos];
    return y;
}

bool isNear(Standard_Real a, Standard_Real b, Standard_Real epsilon)
{
    return (fabs(a - b) <= epsilon);
}

} // namespace geoml
