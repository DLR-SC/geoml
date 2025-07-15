/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-05-08 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "CGroupShapes.h"

#include "CNamedShape.h"
#include "CBooleanOperTools.h"

#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>

namespace geoml
{

CGroupShapes::CGroupShapes(const ListPNamedShape& list)
{
    if (list.size() == 1) {
        _result = list[0];
        return;
    }

    BRep_Builder b;
    TopoDS_Compound c;
    b.MakeCompound(c);

    for (ListPNamedShape::const_iterator it=list.begin(); it != list.end(); ++it) {
        PNamedShape shape = *it;
        if (!shape) {
            continue;
        }

        b.Add(c, shape->Shape());
    }

    _result = PNamedShape(new CNamedShape(c, "ShapeGroup"));

    // apply face names from the shapes
    for (ListPNamedShape::const_iterator it=list.begin(); it != list.end(); ++it) {
        PNamedShape shape = *it;
        if (!shape) {
            continue;
        }

        CBooleanOperTools::AppendNamesToShape(shape, _result);
    }
}

CGroupShapes::~CGroupShapes()
{
}

geoml::CGroupShapes::operator PNamedShape()
{
    return NamedShape();
}

const PNamedShape CGroupShapes::NamedShape()
{
    return _result;
}

} // namespace geoml
