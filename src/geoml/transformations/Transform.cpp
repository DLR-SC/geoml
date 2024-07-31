#include "geoml/transformations/Transform.h"
#include <geometry/Transformation.h>


namespace geoml{

Transform::Transform() : pImpl(std::make_unique<Transformation>()) {}

Transform::Transform(const gp_GTrsf& ocMatrix) : pImpl(std::make_unique<Transformation>(ocMatrix)) {}

Transform::Transform(const gp_Trsf& trans) : pImpl(std::make_unique<Transformation>(trans)) {}

Transform::Transform(const gp_Vec& translation) : pImpl(std::make_unique<Transformation>(translation)) {}

// Destructor
Transform::~Transform() = default;


// copy constructor
Transform::Transform(const Transform& other) 
    : pImpl(std::make_unique<Transformation>(*other.pImpl)) {}

// copy assignment operator
Transform& Transform::operator=(const Transform& other) {
    if (this == &other) {
        return *this; 
    }
    pImpl = std::make_unique<Transformation>(*other.pImpl); 
    return *this;
}

// move constructor
Transform::Transform(Transform&& other) = default;

// move assignment operator
Transform& Transform::operator=(Transform&& other) = default; 


void Transform::PreMultiply(const Transform& aTrans){
    pImpl->PreMultiply(*(aTrans.pImpl));
}

Transform operator*(const Transform &a, const Transform &b)
{
    Transform result; 
    result = b;
    result.PreMultiply(a);
    return result;
}

TopoDS_Shape Transform::Apply_Transform(const TopoDS_Shape& shape) const {
    return pImpl->Transform(shape);
}

gp_Pnt Transform::Apply_Transform(const gp_Pnt& point) const {
    return pImpl->Transform(point);
}

Handle(Geom_Surface) Transform::Apply_Transform(const Handle(Geom_Surface)& surf) const {
    return pImpl->Transform(surf);
}

gp_Vec Transform::Apply_Transform(const gp_Vec& vec) const {
    return pImpl->Transform(vec);
}

} // namespace geoml