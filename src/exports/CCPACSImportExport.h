/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2010-08-13 Markus Litz <Markus.Litz@dlr.de>
*/

#ifndef CCPACSIMPORTEXPORT_H
#define CCPACSIMPORTEXPORT_H

#include "stringtools.h"
#include "geoml/error.h"
#include <string>

namespace geoml
{

enum ShapeGroupMode
{
    WHOLE_SHAPE,           /** Inserts the shape as it is into IGES/STEP. All faces will be named correctly but they will not be grouped by name */
    NAMED_COMPOUNDS,       /** Collects all faces with the same origin into compounds. All faces are named correctly */
    FACES                  /** Exports each face as its own group. The group name and the face name are identical    */
};

template <>
inline void from_string<ShapeGroupMode>(const std::string &s, ShapeGroupMode &t)
{
    std::string value = geoml::to_upper(s);
    if (value == "WHOLE_SHAPE") {
        t = WHOLE_SHAPE;
    }
    else if (value == "NAMED_COMPOUNDS") {
        t = NAMED_COMPOUNDS;
    }
    else if (value == "FACES") {
        t = FACES;
    }
    else {
        throw Error("Cannot convert string to ShapeGroupMode");
    }
}


} // end namespace geoml

#endif // CCPACSIMPORTEXPORT_H
