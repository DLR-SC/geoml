/*
* Copyright (C) 2015 German Aerospace Center (DLR/SC)
*
* Created: 2015-05-08 Martin Siggel <Martin.Siggel@dlr.de>
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

#ifndef CGROUPSHAPES_H
#define CGROUPSHAPES_H

#include "geoml_internal.h"
#include "PNamedShape.h"
#include "ListPNamedShape.h"

namespace geoml
{

class CGroupShapes
{
public:
    // groups multiple shapes into one shape by creating a TopoDS_Compound
    GEOML_EXPORT CGroupShapes(const ListPNamedShape& shapeList);
    GEOML_EXPORT virtual ~CGroupShapes();

    GEOML_EXPORT operator PNamedShape ();

    GEOML_EXPORT const PNamedShape NamedShape();

private:
    PNamedShape _result;
};

} // namespace geoml

#endif // CGROUPSHAPES_H
