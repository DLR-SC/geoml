/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2014-02-15 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef TYPEREGISTRY_H
#define TYPEREGISTRY_H

#include "geoml_internal.h"
/**
 * The registry allows classes to enrigster class functions, that
 * should be run automatically after loading geoml.
 * This mechanism can be used e.g. to register classes at class factories.
 * 
 * In order to make use of the mechanism, implement an AUTOSTART function inside the
 * classes cpp file i.e.:
 * 
 * MyClass.cpp:
 * 
 *    AUTORUN(MyClass)
 *    {
 *        // your code here
 *        return true;
 *    }
 * 
 * Then, this autorun function has to be registered inside the registry. This is done
 * by placing
 *    REGISTER_TYPE(MyClass)
 * inside TypeRegistry.cpp.
 */

namespace geoml
{

class TypeRegistry
{
public:
    GEOML_EXPORT static void Init();
};

}

#define AUTORUN(type) \
    bool type ## _RegisterType()

#endif // TYPEREGISTRY_H
