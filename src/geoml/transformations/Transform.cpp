#include "geoml/transformations/Transform.h"
#include <geometry/Transformation.h>


namespace geoml{

class Transform::Impl {
public:
    Impl() : transformation() {} 
    Impl(const gp_GTrsf& ocMatrix) : transformation(ocMatrix) {}
    Impl(const gp_Trsf& trans) : transformation(trans) {}
    Impl(const gp_Vec& translation) : transformation(translation) {}

    Transformation transformation;
};


Transform::Transform() : pImpl(std::make_unique<Impl>()) {}

Transform::Transform(const gp_GTrsf& ocMatrix) : pImpl(std::make_unique<Impl>(ocMatrix)) {}

Transform::Transform(const gp_Trsf& trans) : pImpl(std::make_unique<Impl>(trans)) {}

Transform::Transform(const gp_Vec& translation) : pImpl(std::make_unique<Impl>(translation)) {}

// Destructor
Transform::~Transform() = default;

// copy constructor
Transform::Transform(const Transform& other) 
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

// copy assignment operator
Transform& Transform::operator=(const Transform& other) {
    if (this == &other) {
        return *this; 
    }
    pImpl = std::make_unique<Impl>(*other.pImpl); 
    return *this;
}

// move constructor
Transform::Transform(Transform&& other) = default;

// move assignment operator
Transform& Transform::operator=(Transform&& other) = default; 


void Transform::PreMultiply(const Transform& aTrans){
    pImpl->transformation.PreMultiply(aTrans.pImpl->transformation);
}

Transform operator*(const Transform &a, const Transform &b)
{
    Transform result; 
    result = b;
    result.PreMultiply(a);
    return result;
}

TopoDS_Shape Transform::Apply_Transform(const TopoDS_Shape& shape) const {
    return pImpl->transformation.Transform(shape);
}

gp_Pnt Transform::Apply_Transform(const gp_Pnt& point) const {
    return pImpl->transformation.Transform(point);
}

Handle(Geom_Surface) Transform::Apply_Transform(const Handle(Geom_Surface)& surf) const {
    return pImpl->transformation.Transform(surf);
}

gp_Vec Transform::Apply_Transform(const gp_Vec& vec) const {
    return pImpl->transformation.Transform(vec);
}

} // namespace geoml