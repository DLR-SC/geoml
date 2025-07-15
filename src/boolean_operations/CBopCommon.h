/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2020 German Aerospace Center (DLR)
*
* Created: 2015-06-15 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef CBOPCOMMON_H
#define CBOPCOMMON_H


#include "PNamedShape.h"
#include "geoml_internal.h"

class BOPAlgo_PaveFiller;

class CBopCommon
{
public:
    // the trimming tool must be a solid!
    GEOML_EXPORT CBopCommon(const PNamedShape shape, const PNamedShape tool);
    GEOML_EXPORT CBopCommon(const PNamedShape shape, const PNamedShape tool, const BOPAlgo_PaveFiller&);
    GEOML_EXPORT virtual ~CBopCommon();

    GEOML_EXPORT operator PNamedShape ();

    GEOML_EXPORT void Perform();
    GEOML_EXPORT const PNamedShape NamedShape();

protected:
    void PrepareFiller();

    bool _hasPerformed;

    PNamedShape _resultshape, _tool, _source;
    BOPAlgo_PaveFiller* _dsfiller;
    bool _fillerAllocated;

};

#endif // CBOPCOMMON_H
