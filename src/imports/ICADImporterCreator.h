/*
* Copyright (C) 2015 German Aerospace Center (DLR/SC)
*
* Created: 2015-05-08 Martin Siggel <Martin.Siggel@dlr.de>
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
