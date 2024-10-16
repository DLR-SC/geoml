/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-05-08 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef ICADIMPORTERCREATOR_H
#define ICADIMPORTERCREATOR_H

#include "geoml_internal.h"
#include "ICADImporter.h"

#include <memory>

namespace geoml
{

class ICADImporterCreator
{
public:
    virtual std::unique_ptr<ICADImporter> create() const = 0;
    virtual ~ICADImporterCreator(){}
};

template <class T>
class ICADImporterCreatorImpl : public ICADImporterCreator
{
public:
   std::unique_ptr<ICADImporter> create() const override
   {
       return std::make_unique<T>();
   }
};

}

#endif // ICADIMPORTERCREATOR_H
