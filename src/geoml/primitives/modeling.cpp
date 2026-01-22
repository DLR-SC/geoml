#include "primitives/modeling.hpp"

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>

namespace geoml{

Shape create_cylinder(Standard_Real radius, Standard_Real height) {
    return Shape(BRepPrimAPI_MakeCylinder(radius, height));
}

Shape create_box(Standard_Real dx, Standard_Real dy, Standard_Real dz) {
    return Shape(BRepPrimAPI_MakeBox(dx,dy,dz));
}

} // namespace geoml

