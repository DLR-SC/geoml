/* 
* Copyright (C) 2016 German Aerospace Center (DLR/SC)
*
* Created: 2016-15-01 Martin Siggel <Martin.Siggel@dlr.de>
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
