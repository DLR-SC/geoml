/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2016 German Aerospace Center (DLR)
*
* Created: 2016-15-01 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef PROJECTONLINEARSPLINE_H
#define PROJECTONLINEARSPLINE_H

#include "geoml_internal.h"

#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>

namespace geoml
{

class ProjectOnLinearSpline
{
public:
    GEOML_EXPORT ProjectOnLinearSpline(Handle(Geom_BSplineCurve) curve, gp_Pnt p);

    /// If false, no projection could be computed
    GEOML_EXPORT bool IsDone();

    /// Returns the point projected
    GEOML_EXPORT gp_Pnt Point();

    /// Returns the curve parameter of the projected point
    GEOML_EXPORT double Parameter();

    GEOML_EXPORT ~ProjectOnLinearSpline();

private:
    bool compute();

    Handle(Geom_BSplineCurve) _curve;
    bool   _isDone;
    gp_Pnt _pProj, _point;
    double _param;
};

}

#endif // PROJECTONLINEARSPLINE_H
