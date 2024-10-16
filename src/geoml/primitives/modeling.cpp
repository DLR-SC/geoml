/**
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 */

#include "primitives/modeling.hpp"

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

namespace geoml{

Shape create_cylinder(double radius, double height) {
    return Shape(BRepPrimAPI_MakeCylinder(radius, height));
}

Shape create_box(double dx, double dy, double dz) {
    return Shape(BRepPrimAPI_MakeBox(dx,dy,dz));
}

} // namespace geoml

