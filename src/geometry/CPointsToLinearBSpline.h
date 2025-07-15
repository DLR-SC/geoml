/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-06-01 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef CPOINTSTOLINEARBSPLINE_H
#define CPOINTSTOLINEARBSPLINE_H

#include "geoml_internal.h"

#include <vector>
#include <gp_Pnt.hxx>

#include <Geom_BSplineCurve.hxx>

namespace geoml
{

/**
 * @brief This class creates a linaer B-Spline interpolation
 * of the given points.
 */
class CPointsToLinearBSpline
{
public:
    GEOML_EXPORT CPointsToLinearBSpline(const std::vector<gp_Pnt>& points);

    GEOML_EXPORT Handle(Geom_BSplineCurve) Curve() const;

    GEOML_EXPORT operator Handle(Geom_BSplineCurve)() const;

    GEOML_EXPORT ~CPointsToLinearBSpline();

private:
    const std::vector<gp_Pnt>& _points;
};


} // namespace geoml

#endif // CPOINTSTOLINEARBSPLINE_H
