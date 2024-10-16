/**
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 */

#include <geoml.h>

#include <TypeRegistry.h>

void geoml::init()
{
    TypeRegistry::Init();
}
