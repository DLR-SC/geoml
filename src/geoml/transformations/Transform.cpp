#include "geoml/transformations/Transform.h"


namespace geoml{

Transform::Transform() {
    Transformation tf;
    m_trsf = tf; 
}

Transform::Transform(const gp_GTrsf& ocMatrix){
    Transformation tf (ocMatrix);
    m_trsf = tf;
}

Transform::Transform(const gp_Trsf& trans){
    Transformation tf (trans);
    m_trsf = tf;
}

Transform::Transform(const gp_Vec& translation){
    Transformation tf (translation);
    m_trsf = tf;
}

void Transform::PreMultiply(const Transform& aTrans){
    m_trsf.PreMultiply(aTrans.m_trsf);
}

Transform operator*(const Transform &a, const Transform &b)
{
    Transform result = b;
    result.PreMultiply(a);
    return result;
}

} // namespace geoml