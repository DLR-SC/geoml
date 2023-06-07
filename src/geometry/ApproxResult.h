/*
* Copyright (C) 2020 German Aerospace Center (DLR/SC)
*
* Created: 2020-07-15 Martin Siggel <Martin.Siggel@dlr.de>
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
