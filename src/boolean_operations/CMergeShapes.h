/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef CMERGESHAPES_H
#define CMERGESHAPES_H

#include "PNamedShape.h"
#include "geoml_internal.h"

class CMergeShapes
{
public:
    // merges shapes with adjacent faces
    GEOML_EXPORT CMergeShapes(const PNamedShape shape, const PNamedShape tool);
    GEOML_EXPORT virtual ~CMergeShapes();

    GEOML_EXPORT operator PNamedShape ();

    GEOML_EXPORT void Perform();
    GEOML_EXPORT const PNamedShape NamedShape();

protected:
    bool _hasPerformed;

    PNamedShape _resultshape, _tool, _source;
};

#endif // CMERGESHAPES_H
