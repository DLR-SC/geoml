/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
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
