/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2020 German Aerospace Center (DLR)
*
* Created: 2020-07-15 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef APPROXRESULT_H
#define APPROXRESULT_H

#include <Geom_BSplineCurve.hxx>

namespace geoml
{

struct ApproxResult
{
    Handle(Geom_BSplineCurve) curve;
    double error;
};

} // namespace geoml

#endif // APPROXRESULT_H
