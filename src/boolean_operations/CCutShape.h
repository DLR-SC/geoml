/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef CCUTSHAPE_H
#define CCUTSHAPE_H

#include "PNamedShape.h"
#include "geoml_internal.h"

class BOPAlgo_PaveFiller;

class CCutShape
{
public:
    // the trimming tool must be a solid!
    GEOML_EXPORT CCutShape(const PNamedShape shape, const PNamedShape cuttingTool);
    GEOML_EXPORT CCutShape(const PNamedShape shape, const PNamedShape cuttingTool, const BOPAlgo_PaveFiller&);
    GEOML_EXPORT virtual ~CCutShape();

    GEOML_EXPORT operator PNamedShape ();

    GEOML_EXPORT void Perform();
    
    /**
     * @brief NamedShape returns the result of the cutting operation.
     * @return PNamedShape
     */
    GEOML_EXPORT const PNamedShape NamedShape();

protected:
    void PrepareFiller();

    bool _hasPerformed;

    PNamedShape _resultshape, _tool, _source;
    BOPAlgo_PaveFiller* _dsfiller;
    bool _fillerAllocated;

};

#endif // CCUTSHAPE_H
