/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-06-01 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "CPointsToLinearBSpline.h"

#include <Geom_BSplineCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

namespace geoml
{

CPointsToLinearBSpline::CPointsToLinearBSpline(const std::vector<gp_Pnt>& points)
    : _points(points)
{
}

Handle(Geom_BSplineCurve) CPointsToLinearBSpline::Curve() const
{
    int ncp = (int) _points.size();
    std::vector<gp_Pnt>::const_iterator iter;

    // compute total length and copy control points
    int icp = 1;
    gp_Pnt lastP = _points[0];
    double totalLen = 0.;
    TColgp_Array1OfPnt cp(1, ncp);
    for (iter = _points.begin(); iter != _points.end(); ++iter) {
        gp_Pnt p = *iter;
        double segLen = p.Distance(lastP);
        totalLen += segLen;
        cp.SetValue(icp++, p);
        lastP = p;
    }

    // compute knots
    TColStd_Array1OfReal knots(1, ncp);
    TColStd_Array1OfInteger mults(1, ncp);

    lastP = _points[0];
    double lastKnot = 0;
    for (int i = 1; i <= ncp; ++i) {
        if (i == 1 || i == ncp) {
            mults.SetValue(i,2);
        }
        else {
            mults.SetValue(i,1);
        }

        double knot = cp.Value(i).Distance(lastP)/totalLen + lastKnot;
        knots.SetValue(i, knot);

        lastKnot = knot;
        lastP = cp.Value(i);
    }

    return new Geom_BSplineCurve(cp, knots, mults, 1, false);
}

CPointsToLinearBSpline::operator Handle(Geom_BSplineCurve)() const
{
    return Curve();
}

CPointsToLinearBSpline::~CPointsToLinearBSpline()
{
}


} // namespace geoml
