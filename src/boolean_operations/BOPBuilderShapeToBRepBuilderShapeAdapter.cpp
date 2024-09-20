/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "BOPBuilderShapeToBRepBuilderShapeAdapter.h"

#include <BOPAlgo_BuilderShape.hxx>

BOPBuilderShapeToBRepBuilderShapeAdapter::BOPBuilderShapeToBRepBuilderShapeAdapter(BOPAlgo_BuilderShape& adaptee)
    : _adaptee(adaptee)
{
}

const TopoDS_Shape &BOPBuilderShapeToBRepBuilderShapeAdapter::Shape() const
{
    return _adaptee.Shape();
}

BOPBuilderShapeToBRepBuilderShapeAdapter::operator TopoDS_Shape() const
{
    return Shape();
}


const TopTools_ListOfShape& BOPBuilderShapeToBRepBuilderShapeAdapter::Generated(const TopoDS_Shape &S)
{
    return _adaptee.Generated(S);
}

const TopTools_ListOfShape& BOPBuilderShapeToBRepBuilderShapeAdapter::Modified(const TopoDS_Shape &S)
{
    return _adaptee.Modified(S);
}

Standard_Boolean BOPBuilderShapeToBRepBuilderShapeAdapter::IsDeleted(const TopoDS_Shape &S)
{
    return _adaptee.IsDeleted(S);
}
