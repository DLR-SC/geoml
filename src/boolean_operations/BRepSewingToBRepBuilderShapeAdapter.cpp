/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "BRepSewingToBRepBuilderShapeAdapter.h"

#include <BRepBuilderAPI_Sewing.hxx>
#include <TopExp.hxx>

BRepSewingToBRepBuilderShapeAdapter::BRepSewingToBRepBuilderShapeAdapter(BRepBuilderAPI_Sewing& adaptee)
    : _adaptee(adaptee)
{
}

const TopoDS_Shape &BRepSewingToBRepBuilderShapeAdapter::Shape() const
{
    return _adaptee.SewedShape();
}

BRepSewingToBRepBuilderShapeAdapter::operator TopoDS_Shape() const
{
    return Shape();
}

const TopTools_ListOfShape& BRepSewingToBRepBuilderShapeAdapter::Modified(const TopoDS_Shape &S)
{
    _modified.Clear();
    TopoDS_Shape modshape = _adaptee.Modified(S);
    TopTools_IndexedMapOfShape map;
    TopExp::MapShapes(modshape, TopAbs_FACE, map);
    for (int iface = 1; iface <= map.Extent(); ++iface) {
        _modified.Append(map(iface));
    }

    return _modified;
}
