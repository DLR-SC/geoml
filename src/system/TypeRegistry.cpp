/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2014-02-15 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "TypeRegistry.h"

#define REGISTER_TYPE(type) \
    AUTORUN(type); \
    static const bool registered_ ## type = type ## _RegisterType();

namespace geoml
{

// register all dynamic types to prevent linker optimization
REGISTER_TYPE(ImportStep)
REGISTER_TYPE(ExportStep)
REGISTER_TYPE(ExportIges)
REGISTER_TYPE(ExportStl)
REGISTER_TYPE(ExportBrep)


void TypeRegistry::Init()
{
// dummy implementation
}
 
} // namespace geoml
