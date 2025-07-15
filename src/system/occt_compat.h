/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-01-05 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef OCCT_COMPAT_H
#define OCCT_COMPAT_H

#include "geoml_internal.h"
#include <Standard_Version.hxx>

#if OCC_VERSION_HEX < 0x070000
  #define DEFINE_STANDARD_RTTI_OVERRIDE(C1) \
     Standard_EXPORT const Handle(Standard_Type)& DynamicType() const override;
  #define DEFINE_STANDARD_RTTIEXT(C1,C2) DEFINE_STANDARD_RTTI_OVERRIDE(C1)
  #define DEFINE_STANDARD_RTTI_INLINE(C1,C2) DEFINE_STANDARD_RTTI_OVERRIDE(C1)
#endif

#endif // OCCT_COMPAT_H

