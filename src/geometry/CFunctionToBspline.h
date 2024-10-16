/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2014 German Aerospace Center (DLR)
*
* Created: 2014-11-17 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef FUNCTIONTOBSPLINE_H
#define FUNCTIONTOBSPLINE_H

#include "math/MathFunctions.h"
#include "geoml_internal.h"

#include <Geom_BSplineCurve.hxx>

namespace geoml
{

/**
 * @brief The FunctionToBspline class can be used to approximate an arbitrary 3D function
 * with a B-Spline. 
 * 
 * The function to approximate is given by 3 separate functions, one for the x values,
 * one for y-values and one for the z-values.
 */
class GEOML_EXPORT CFunctionToBspline
{
public:
    /**
     * @brief CFunctionToBspline - Constructor
     *
     * @param f Function returning the values of the curve
     * @param umin Start parameter of the function to approximate
     * @param umax End parameter of the function to approximate
     * @param degree Degree of the resulting B-Spline
     * @param tolerance Maximum approximation error
     * @param maxDepth Maximum depth of curve splitting -> Influences resulting segment number
     */
    CFunctionToBspline(MathFunc3d& f,
                       double umin, double umax,
                       int degree,
                       double tolerance=1e-5,
                       int maxDepth = 10);

    ~CFunctionToBspline();

    /// Computes the B-Spline approximation
    Handle(Geom_BSplineCurve) Curve();

    /// returns the error made by spline approximation
    double ApproxError();

private:
    class CFunctionToBsplineImpl;
    CFunctionToBsplineImpl* pimpl;
};

} // namespace geoml

#endif // FUNCTIONTOBSPLINE_H
