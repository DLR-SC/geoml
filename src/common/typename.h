/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2016 RISC Software GmbH
*/

#ifndef TYPENAME_H
#define TYPENAME_H

#include <string>
#include <typeinfo>

#include "geoml_internal.h"

namespace geoml
{

GEOML_EXPORT std::string typeName(const std::type_info& ti);

template <typename T>
std::string typeName()
{
    return typeName(typeid(T));
}

}

#endif // TYPENAME_H
