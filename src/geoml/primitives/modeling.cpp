#include "primitives/modeling.h"

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

