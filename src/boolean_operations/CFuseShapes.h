/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef CFUSESHAPES_H
#define CFUSESHAPES_H

#include "PNamedShape.h"
#include "ListPNamedShape.h"
#include "geoml_internal.h"

/**
 * @brief CFuseShapes Implement a fuse, where the childs are fused with the parent
 *
 * This seems to be the most performant version so far as we can reuse a dsfiller
 * for trimming a parent and a child. It performs at the same speed as the original
 * occt fuse, but generates more reliable values.
 *
 * The function works only for solids!!!
 *
 */
class CFuseShapes
{
public:
    // all shapes must be solids!!!
    GEOML_EXPORT CFuseShapes(const PNamedShape parent, const ListPNamedShape& childs);
    GEOML_EXPORT virtual ~CFuseShapes();

    // returns result of fusing operation
    GEOML_EXPORT operator PNamedShape ();
    GEOML_EXPORT const    PNamedShape NamedShape();

    GEOML_EXPORT const ListPNamedShape& Intersections();
    GEOML_EXPORT const ListPNamedShape& TrimmedChilds();
    GEOML_EXPORT const PNamedShape   TrimmedParent();

    GEOML_EXPORT void Perform();


protected:
    void Clear();
    void DoFuse();

    bool _hasPerformed;

    PNamedShape _resultshape, _parent, _trimmedParent;
    ListPNamedShape _childs, _trimmedChilds, _intersections;
};
#endif // CFUSESHAPES_H
