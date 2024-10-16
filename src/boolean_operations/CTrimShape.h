/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef CTRIMSHAPE_H
#define CTRIMSHAPE_H

#include "PNamedShape.h"
#include "geoml_internal.h"

class BOPAlgo_PaveFiller;

enum TrimOperation
{
    EXCLUDE = 0,
    INCLUDE = 1
};

class CTrimShape
{
public:
    // the trimming tool must be a solid!
    GEOML_EXPORT CTrimShape(const PNamedShape shape, const PNamedShape trimmingTool, TrimOperation = EXCLUDE);
    GEOML_EXPORT CTrimShape(const PNamedShape shape, const PNamedShape trimmingTool, const BOPAlgo_PaveFiller&, TrimOperation = EXCLUDE);
    GEOML_EXPORT virtual ~CTrimShape();

    GEOML_EXPORT operator PNamedShape ();

    GEOML_EXPORT void Perform();
    GEOML_EXPORT const PNamedShape NamedShape();

protected:
    void PrepareFiller();

    bool _hasPerformed;
    TrimOperation _operation;

    PNamedShape _resultshape, _tool, _source;
    BOPAlgo_PaveFiller* _dsfiller;
    bool _fillerAllocated;

};

#endif // CTRIMSHAPE_H
