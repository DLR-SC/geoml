/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef BREPSEWINGTOBREPBUILDERSHAPEADAPTER_H
#define BREPSEWINGTOBREPBUILDERSHAPEADAPTER_H

#include <BRepBuilderAPI_MakeShape.hxx>
#include "geoml_internal.h"

class BRepBuilderAPI_Sewing;

class BRepSewingToBRepBuilderShapeAdapter : public BRepBuilderAPI_MakeShape
{
public:
    GEOML_EXPORT BRepSewingToBRepBuilderShapeAdapter(BRepBuilderAPI_Sewing& adaptee);

    GEOML_EXPORT const TopoDS_Shape& Shape() const;
    GEOML_EXPORT operator TopoDS_Shape() const;

    GEOML_EXPORT const TopTools_ListOfShape& Modified(const TopoDS_Shape& S) override;

private:
    BRepBuilderAPI_Sewing& _adaptee;
    TopTools_ListOfShape _modified;
};

#endif // BREPSEWINGTOBREPBUILDERSHAPEADAPTER_H
