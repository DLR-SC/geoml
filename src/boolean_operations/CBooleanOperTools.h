/*
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
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

#ifndef CBOOLEANOPERTOOLS_H
#define CBOOLEANOPERTOOLS_H

#include "PNamedShape.h"
#include "geoml_internal.h"

class BRepBuilderAPI_MakeShape;

// collection of static function
class CBooleanOperTools
{
public:
    /// maps the face names of source to the facenames of
    /// the resultShape as a result of a boolean operation bop
    GEOML_EXPORT static void MapFaceNamesAfterBOP(BRepBuilderAPI_MakeShape& bop, const PNamedShape source, PNamedShape result);


     /// AppendNamesToShape searches for each target face, if it
     /// can be found in the source shape. If so, it applies the name of
     /// the source face to the target face
    GEOML_EXPORT static void AppendNamesToShape(const PNamedShape source, PNamedShape target);

    /// Tries to sew adjacent faces to create a shell (equivalent to create wires)
    GEOML_EXPORT static PNamedShape Shellify(PNamedShape shape);
};

#endif // CBOOLEANOPERTOOLS_H
