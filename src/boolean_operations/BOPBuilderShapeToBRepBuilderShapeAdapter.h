/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef BOPBUILDERSHAPETOBREPBUILDERSHAPEADAPTER_H
#define BOPBUILDERSHAPETOBREPBUILDERSHAPEADAPTER_H

#include <BRepBuilderAPI_MakeShape.hxx>
#include "geoml_internal.h"

class BOPAlgo_BuilderShape;

class BOPBuilderShapeToBRepBuilderShapeAdapter : public BRepBuilderAPI_MakeShape
{
public:
    GEOML_EXPORT BOPBuilderShapeToBRepBuilderShapeAdapter(BOPAlgo_BuilderShape& adaptee);

    GEOML_EXPORT const TopoDS_Shape& Shape() const;
    GEOML_EXPORT operator TopoDS_Shape() const;

    GEOML_EXPORT const TopTools_ListOfShape& Generated(const TopoDS_Shape& S) override;
    GEOML_EXPORT const TopTools_ListOfShape& Modified(const TopoDS_Shape& S) override;
    GEOML_EXPORT Standard_Boolean IsDeleted(const TopoDS_Shape& S) override;

private:
    BOPAlgo_BuilderShape& _adaptee;
};

#endif // BOPBUILDERSHAPETOBREPBUILDERSHAPEADAPTER_H
