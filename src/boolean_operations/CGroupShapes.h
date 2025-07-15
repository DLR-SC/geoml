/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-05-08 Martin Siggel <Martin.Siggel@dlr.de>
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
