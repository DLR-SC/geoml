/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2019 German Aerospace Center (DLR)
*
* Created: 2019-01-17 Jan Kleinert <Jan.Kleinert@dlr.de>
*/

#pragma once

#include "geoml_internal.h"

#include <math_Matrix.hxx>

namespace geoml
{
typedef math_Matrix Matrix;
typedef math_Vector Vector;

/// Stores the field to a file in binary form
/// Note: this is not portable. The result will
/// be different on varying platforms.
///
/// The corresponding read function should be used only on the same system
GEOML_EXPORT void writeMatrix(const Matrix& x, const std::string& filename);


GEOML_EXPORT Matrix readMatrix(const std::string& filename);

}
