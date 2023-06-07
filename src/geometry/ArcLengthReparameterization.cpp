/*
* Copyright (C) 2017 German Aerospace Center (DLR/SC)
*
* Created: 2017-01-18 Martin Siggel <Martin.Siggel@dlr.de>
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

#include "ArcLengthReparameterization.h"

#include "CFunctionToBspline.h"
#include "math/MathFunctions.h"
#include "geoml/error.h"

#include <GCPnts_AbscissaPoint.hxx>
#include <GeomAdaptor_Curve.hxx>

namespace geoml
{

class BSplineArcLength : public MathFunc3d
{
public:
    BSplineArcLength(const Handle(Geom_BSplineCurve) curve)
        : _curve(curve)
    {
        // intentionally empty
    }

    double getParameter(double arcLength);

    // returns the total length of the spline
    double getTotalLength();

    double valueX(double t) override {
        return getParameter(t);
    }

    double valueY(double t) override
    {
        return 0.;
    }
    double valueZ(double t) override
    {
        return 0.;
    }

private:
    GeomAdaptor_Curve _curve;
};

double BSplineArcLength::getParameter(double arcLength)
{
    GCPnts_AbscissaPoint algo(_curve, arcLength, _curve.FirstParameter());
    if (algo.IsDone()) {
        return algo.Parameter();
    }
    else {
        throw geoml::Error("BSplineArcLength: can't compute parameter for arc given length.", geoml::MATH_ERROR);
    }
}

double BSplineArcLength::getTotalLength()
{
    return GCPnts_AbscissaPoint::Length(_curve);
}


ReparametrizeCurveByArclen::ReparametrizeCurveByArclen()
    : _isInitialized(false)
{
}

bool ReparametrizeCurveByArclen::isInitialized() const
{
    return _isInitialized;
}

double ReparametrizeCurveByArclen::parameter(double arcLength) const
{
    if (!isInitialized()) {
        throw Error("ReparametrizeCurveByArclen is not initialized", geoml::MATH_ERROR);
    }

    return _reparCurve->Value(arcLength).X();
}

double ReparametrizeCurveByArclen::totalLength() const
{
    if (!isInitialized()) {
        throw Error("ReparametrizeCurveByArclen is not initialized", geoml::MATH_ERROR);
    }

    return _totalLength;
}

void ReparametrizeCurveByArclen::init(const Handle(Geom_BSplineCurve) curve, double accuracy)
{
    _isInitialized = false;

    BSplineArcLength arcLenFun(curve);

    // Compute total arc length
    _totalLength = arcLenFun.getTotalLength();

    CFunctionToBspline approximator(arcLenFun, 0., _totalLength, 3, accuracy/10.);
    _reparCurve = approximator.Curve();

    _isInitialized = true;
}

} // namespace geoml
