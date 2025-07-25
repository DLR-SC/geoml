/*
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
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
