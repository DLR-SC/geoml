/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2017 German Aerospace Center (DLR)
*
* Created: 2017-01-18 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef ARCLENGTHREPARAMETERIZATION_H
#define ARCLENGTHREPARAMETERIZATION_H

#include "geoml_internal.h"

#include <Geom_BSplineCurve.hxx>

namespace geoml
{

/**
 * @brief This provides an algorithm for an arc-length
 * reparameterization of a bspline curve
 *
 * This class uses the techniques from the paper
 * Peterson J. Arc Length Parameterization of Spline Curves
 */
class ReparametrizeCurveByArclen
{
public:
    GEOML_EXPORT ReparametrizeCurveByArclen();

    GEOML_EXPORT void init(const Handle(Geom_BSplineCurve) curve, double accuracy=1e-5);

    GEOML_EXPORT bool isInitialized() const;

    /// Returns the curve parameter for the given arc length
    GEOML_EXPORT double parameter(double arcLength) const;

    GEOML_EXPORT double totalLength() const;

private:
    Handle(Geom_BSplineCurve) _reparCurve;

    bool _isInitialized;
    double _totalLength;
};

} // namespace geoml

#endif // ARCLENGTHREPARAMETERIZATION_H
