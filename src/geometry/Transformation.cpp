/* 
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2010-08-13 Markus Litz <Markus.Litz@dlr.de>
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
/**
* @file
* @brief  Implementation of geoml transformations.
*/

#include "geometry/Transformation.h"
#include "geoml/error.h"
#include "BRepBuilderAPI_GTransform.hxx"
#include "BRepBuilderAPI_Transform.hxx"
#include "BRepBuilderAPI_MakeFace.hxx"
#include "BRep_Tool.hxx"
#include "gp_XYZ.hxx"
#include "TopoDS_Face.hxx"
#include "Standard_Version.hxx"
#include "CNamedShape.h"

#include "math/MathFunctions.h"

namespace geoml 
{

// Constructor
Transformation::Transformation()
{
    SetIdentity();
}

Transformation::Transformation(const gp_GTrsf& ocMatrix) 
{
    SetIdentity();
    
    // Vectorial part
    for (int i = 0; i < 3; ++i) {
        for (unsigned j = 0; j < 3; ++j) {
            m_matrix[i][j] = ocMatrix.Value(i+1,j+1);
        }
    }

    // Translation part
    m_matrix[0][3] = ocMatrix.Value(1,4);
    m_matrix[1][3] = ocMatrix.Value(2,4);
    m_matrix[2][3] = ocMatrix.Value(3,4);
}

// Constructor for transformation based on gp_Trsf
Transformation::Transformation(const gp_Trsf& trans)
{
    SetIdentity();

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
            m_matrix[row][col] = trans.Value(row+1, col+1);
        }
    }
}

Transformation::Transformation(const gp_Vec& t)
{
    SetIdentity();

    AddTranslation(t.X(), t.Y(), t.Z());
}

Transformation& Transformation::operator=(const Transformation& mat)
{
    for (unsigned int i = 0; i < 4; ++i) {
        for (unsigned int j = 0; j < 4; ++j) {
            m_matrix[i][j] = mat.m_matrix[i][j];
        }
    }
    return *this;
}

void Transformation::SetIdentity()
{
    // preset with 0
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            m_matrix[row][col] = 0.0;
        }
    }

    // set diagonal to 1
    for (int i = 0; i < 4; i++) {
        m_matrix[i][i] = 1.0;
    }
}

// Sets a value of the transformation matrix by row/col
void Transformation::SetValue(int row, int col, double value)
{
    if (row < 0 || row > 3 || col < 0 || col > 3) {
        throw Error("Invalid row or column index in Transformation::SetValue", geoml::INDEX_ERROR);
    }

    m_matrix[row][col] = value;
}

// Post multiply this matrix with another matrix and stores 
// the result in this matrix
void Transformation::PostMultiply(const Transformation& aTrans)
{
    double tmp_matrix[4][4];
    int row;

    for (row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            tmp_matrix[row][col] = 0.0;
            for (int j = 0; j < 4; j++) {
                tmp_matrix[row][col] += m_matrix[row][j] * aTrans.m_matrix[j][col];
            }
        }
    }

    // Store results back
    for (row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            m_matrix[row][col] = tmp_matrix[row][col];
        }
    }
}

// Pre multiply this matrix with another matrix and stores 
// the result in this matrix
void Transformation::PreMultiply(const Transformation& aTrans)
{
    double tmp_matrix[4][4];
    int row;

    for (row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            tmp_matrix[row][col] = 0.0;
            for (int j = 0; j < 4; j++) {
                tmp_matrix[row][col] += aTrans.m_matrix[row][j] * m_matrix[j][col];
            }
        }
    }

    // store results back
    for (row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            m_matrix[row][col] = tmp_matrix[row][col];
        }
    }
}

// Returns the current transformation as gp_GTrsf object
gp_GTrsf Transformation::Get_gp_GTrsf() const
{
    gp_GTrsf ocMatrix;

    // Vectorial part
    ocMatrix.SetValue(1, 1, m_matrix[0][0]);
    ocMatrix.SetValue(1, 2, m_matrix[0][1]);
    ocMatrix.SetValue(1, 3, m_matrix[0][2]);

    ocMatrix.SetValue(2, 1, m_matrix[1][0]);
    ocMatrix.SetValue(2, 2, m_matrix[1][1]);
    ocMatrix.SetValue(2, 3, m_matrix[1][2]);

    ocMatrix.SetValue(3, 1, m_matrix[2][0]);
    ocMatrix.SetValue(3, 2, m_matrix[2][1]);
    ocMatrix.SetValue(3, 3, m_matrix[2][2]);

    // Translation part
    ocMatrix.SetValue(1, 4, m_matrix[0][3]);
    ocMatrix.SetValue(2, 4, m_matrix[1][3]);
    ocMatrix.SetValue(3, 4, m_matrix[2][3]);

    return ocMatrix;
}

// Converts degree to radian, utility function
double Transformation::DegreeToRadian(double degree)
{
    return (degree * 1.74532925199433E-02);
}

// Converts radian to degree, utility function
double Transformation::RadianToDegree(double radian)
{
    return (radian * 57.2957795130823);
}

// Adds a translation to this transformation. Translation part
// is stored in the last column of the transformation matrix.
void Transformation::AddTranslation(double tx, double ty, double tz)
{
    // Matrix is:
    //
    // (      1       0       0       tx )
    // (      0       1       0       ty )
    // (      0       0       1       tz )
    // (      0       0       0        1 )
    
    Transformation trans;
    trans.SetValue(0, 3, tx);
    trans.SetValue(1, 3, ty);
    trans.SetValue(2, 3, tz);

    PreMultiply(trans);
}

void Transformation::AddScaling(double sx, double sy, double sz)
{
    // Matrix is:
    //
    // (     sx       0       0        0 )
    // (      0      sy       0        0 )
    // (      0       0       sz       0 )
    // (      0       0       0        1 )
    
    Transformation trans;
    trans.SetValue(0, 0, sx);
    trans.SetValue(1, 1, sy);
    trans.SetValue(2, 2, sz);

    PreMultiply(trans);
}

void Transformation::AddRotationX(double degreeX)
{
    double radianX = DegreeToRadian(degreeX);
    double sinVal  = sin(radianX);
    double cosVal  = cos(radianX);

    // Matrix is:
    //
    // (      1       0       0        0 )
    // (      0  cosVal -sinVal        0 )
    // (      0  sinVal  cosVal        0 )
    // (      0       0       0        1 )
    
    Transformation trans;
    trans.SetValue(1, 1, cosVal);
    trans.SetValue(1, 2, -sinVal);
    trans.SetValue(2, 1, sinVal);
    trans.SetValue(2, 2, cosVal);

    PreMultiply(trans);
}

void Transformation::AddRotationY(double degreeY)
{
    double radianY = DegreeToRadian(degreeY);
    double sinVal  = sin(radianY);
    double cosVal  = cos(radianY);

    // Matrix is:
    //
    // ( cosVal       0  sinVal        0 )
    // (      0       1       0        0 )
    // (-sinVal       0  cosVal        0 )
    // (      0       0       0        1 )
    
    Transformation trans;
    trans.SetValue(0, 0, cosVal);
    trans.SetValue(0, 2, sinVal);
    trans.SetValue(2, 0, -sinVal);
    trans.SetValue(2, 2, cosVal);

    PreMultiply(trans);
}

void Transformation::AddRotationZ(double degreeZ)
{
    double radianZ = DegreeToRadian(degreeZ);
    double sinVal  = sin(radianZ);
    double cosVal  = cos(radianZ);

    // Matrix is:
    //
    // ( cosVal -sinVal       0        0 )
    // ( sinVal  cosVal       0        0 )
    // (      0       0       1        0 )
    // (      0       0       0        1 )
    
    Transformation trans;
    trans.SetValue(0, 0, cosVal);
    trans.SetValue(0, 1, -sinVal);
    trans.SetValue(1, 0, sinVal);
    trans.SetValue(1, 1, cosVal);

    PreMultiply(trans);
}

// Adds a rotation in intrinsic x-y'-z'' Euler convention to the matrix
void Transformation::AddRotationIntrinsicXYZ(double phi, double theta, double psi)
{
    // intrinsic x-y'-z'' corresponds to extrinsic z-y-x, i.e. Rx*Ry*Rz:
    AddRotationZ(psi);
    AddRotationY(theta);
    AddRotationX(phi);
}

// Adds projection an xy plane by setting the z coordinate to 0
void Transformation::AddProjectionOnXYPlane()
{
    // Matrix is:
    //
    // (      1       0       0        0 )
    // (      0       1       0        0 )
    // (      0       0       0        0 )
    // (      0       0       0        1 )
    
    Transformation trans;
    trans.SetValue(2, 2, 0.0);

    PreMultiply(trans);
}

// Adds projection an xz plane by setting the y coordinate to 0
void Transformation::AddProjectionOnXZPlane()
{
    // Matrix is:
    //
    // (      1       0       0        0 )
    // (      0       0       0        0 )
    // (      0       0       1        0 )
    // (      0       0       0        1 )
    
    Transformation trans;
    trans.SetValue(1, 1, 0.0);

    PreMultiply(trans);
}

// Adds projection an yz plane by setting the x coordinate to 0
void Transformation::AddProjectionOnYZPlane()
{
    // Matrix is:
    //
    // (      0       0       0        0 )
    // (      0       1       0        0 )
    // (      0       0       1        0 )
    // (      0       0       0        1 )
    
    Transformation trans;
    trans.SetValue(0, 0, 0.0);

    PreMultiply(trans);
}

// Adds mirroring at xy plane
void Transformation::AddMirroringAtXYPlane()
{
    // Matrix is:
    //
    // (      1       0       0        0 )
    // (      0       1       0        0 )
    // (      0       0       -1       0 )
    // (      0       0       0        1 )
    
    Transformation trans;
    trans.SetValue(2, 2, -1.0);

    PreMultiply(trans);
}

// Adds mirroring at xz plane
void Transformation::AddMirroringAtXZPlane()
{
    // Matrix is:
    //
    // (      1       0       0        0 )
    // (      0       -1      0        0 )
    // (      0       0       0        0 )
    // (      0       0       0        1 )
    
    Transformation trans;
    trans.SetValue(1, 1, -1.0);

    PreMultiply(trans);
}

// Adds mirroring at yz plane
void Transformation::AddMirroringAtYZPlane()
{
    // Matrix is:
    //
    // (      -1      0       0        0 )
    // (      0       1       0        0 )
    // (      0       0       1        0 )
    // (      0       0       0        1 )
    
    Transformation trans;
    trans.SetValue(0, 0, -1.0);

    PreMultiply(trans);
}

// Transforms a shape with the current transformation matrix and
// returns the transformed shape
TopoDS_Shape Transformation::Transform(const TopoDS_Shape& shape) const
{

    if (IsUniform()) {
        gp_Trsf t;
        t.SetValues(m_matrix[0][0], m_matrix[0][1], m_matrix[0][2], m_matrix[0][3],
                    m_matrix[1][0], m_matrix[1][1], m_matrix[1][2], m_matrix[1][3],
                    m_matrix[2][0], m_matrix[2][1], m_matrix[2][2], m_matrix[2][3]
#if OCC_VERSION_HEX >= VERSION_HEX_CODE(6,8,0)
                );
#else
                ,1e-10, 1e-10);
#endif
        BRepBuilderAPI_Transform trafo(shape, t, Standard_True);
        return trafo.Shape();
    }
    else {
        BRepBuilderAPI_GTransform brepBuilderGTransform(shape, Get_gp_GTrsf(), Standard_True);
        const TopoDS_Shape& transformedShape = brepBuilderGTransform.Shape();
        return transformedShape;
    }
}

// Transforms a surface with the current transformation matrix and
// returns the transformed point
GEOML_EXPORT Handle(Geom_Surface) Transformation::Transform(const Handle(Geom_Surface)& surf) const
{
    auto surfCopy = Handle(Geom_Surface)::DownCast(surf->Copy());

    TopoDS_Face tmpFace = BRepBuilderAPI_MakeFace(surfCopy, 1e-6);
    tmpFace = TopoDS::Face(Transform(tmpFace));
    TopLoc_Location L;
    surfCopy = BRep_Tool::Surface(tmpFace, L);
    surfCopy->Transform(L.Transformation());

    return surfCopy;
}

PNamedShape geoml::Transformation::Transform(PNamedShape shape) const
{
    if (!shape) {
        return nullptr;
    }

    PNamedShape mirrored(new CNamedShape(*shape));
    mirrored->SetShape(Transform(mirrored->Shape()));

    // we have to transform also the face transformation properties
    for (unsigned int iface = 0; iface < mirrored->GetFaceCount(); ++iface) {
        CFaceTraits& traits = mirrored->FaceTraits(iface);
        auto faceTrafo = traits.Transformation();

        faceTrafo = *this * faceTrafo * this->Inverted();
        traits.SetTransformation(faceTrafo);
    }
    return mirrored;
}

// Transforms a point with the current transformation matrix and
// returns the transformed point
gp_Pnt Transformation::Transform(const gp_Pnt& point) const
{
    gp_XYZ transformed(point.X(), point.Y(), point.Z());
    Get_gp_GTrsf().Transforms(transformed);
    return gp_Pnt(transformed.X(), transformed.Y(), transformed.Z());
}


gp_Vec Transformation::Transform(const gp_Vec& vec) const
{
    gp_Pnt pTip = Transform(gp_Pnt(vec.XYZ()));
    gp_Pnt pRoot = Transform(gp_Pnt(0., 0., 0));

    return pTip.XYZ() - pRoot.XYZ();
}


bool Transformation::IsUniform() const
{
    // The following code is copied from gp_Trsf

    gp_XYZ col1(m_matrix[0][0], m_matrix[1][0], m_matrix[2][0]);
    gp_XYZ col2(m_matrix[0][1], m_matrix[1][1], m_matrix[2][1]);
    gp_XYZ col3(m_matrix[0][2], m_matrix[1][2], m_matrix[2][2]);

    // compute the determinant
    gp_Mat M(col1, col2, col3);
    Standard_Real s = M.Determinant();

    if (fabs(s) < Precision::Confusion()) {
        return false;
    }

    if (s > 0) {
        s = Pow(s, 1. / 3.);
    }
    else {
        s = -Pow(-s, 1. / 3.);
    }
    M.Divide(s);

    // check if the matrix is a rotation matrix
    // i.e. check if M^T * M = I
    gp_Mat TM(M);
    TM.Transpose();
    TM.Multiply(M);

    // don t trust the initial values !
    gp_Mat anIdentity;
    anIdentity.SetIdentity();
    TM.Subtract(anIdentity);

    double v = 0;
    v = TM.Value(1, 1);
    if (fabs(v) > Precision::Confusion()) {
        return false;
    }

    v = TM.Value(1, 2);
    if (fabs(v) > Precision::Confusion()) {
        return false;
    }

    v = TM.Value(1, 3);
    if (fabs(v) > Precision::Confusion()) {
        return false;
    }

    v = TM.Value(2, 1);
    if (fabs(v) > Precision::Confusion()) {
        return false;
    }

    v = TM.Value(2, 2);
    if (fabs(v) > Precision::Confusion()) {
        return false;
    }

    v = TM.Value(2, 3);
    if (fabs(v) > Precision::Confusion()) {
        return false;
    }

    v = TM.Value(3, 1);
    if (fabs(v) > Precision::Confusion()) {
        return false;
    }

    v = TM.Value(3, 2);
    if (fabs(v) > Precision::Confusion()) {
        return false;
    }

    v = TM.Value(3, 3);
    if (fabs(v) > Precision::Confusion()) {
        return false;
    }

    return true;
}

Transformation Transformation::Inverted() const
{
    return Transformation(Get_gp_GTrsf().Inverted());
}

void Transformation::Decompose(double scale[3], double rotation[3], double translation[3]) const
{
    // compute polar decomposition of upper 3x3 part
    Matrix A(1, 3, 1, 3);
    Matrix P(1, 3, 1, 3);
    Matrix U(1, 3, 1, 3);
    for( int i=0; i<3; ++i) {
        for ( int j=0; j<3; ++j) {
            A(i+1,j+1)= GetValue(i,j);
        }
    }
    PolarDecomposition(A, U, P);

    // scale is diagonal of P
    scale[0] = P(1,1);
    scale[1] = P(2,2);
    scale[2] = P(3,3);

    // check for shearing
    double aveAbsOffDiag = (fabs(P(1,2)) + fabs(P(1,3)) + fabs(P(2,3)))/3;
    if (aveAbsOffDiag > Precision::Confusion() ) {
        LOG(WARNING) << "Transformation::Decompose: The Transformation contains a Shearing, that will be discarded in the decomposition!";
    }

    // calculate intrinsic Euler angles from rotation matrix U
    //
    // This implementation is based on http://www.gregslabaugh.net/publications/euler.pdf, where the same argumentation
    // is used for intrinsic x,y',z'' angles and the rotatation matrix
    //
    //                |                        cos(y)*cos(z) |               -        cos(y)*cos(z) |         sin(y) |
    // U = Rx*Ry*Rz = | cos(x)*sin(z) + sin(x)*sin(y)*cos(z) | cos(x)*cos(z) - sin(x)*sin(y)*sin(z) | -sin(x)*cos(y) |
    //                | sin(x)*sin(z) - cos(x)*sin(y)*cos(z) | sin(x)*cos(z) + cos(x)*sin(y)*sin*z( |  cos(x)*cos(y) |
    //
    // rather than extrinsic angles and the Rotation matrix mentioned in that pdf.

    if( fabs( fabs(U(1, 3)) - 1) > 1e-10 ){
        rotation[1] = asin(U(1, 3));
        double cosTheta = cos(rotation[1]);
        rotation[0] = -atan2(U(2,3)/cosTheta, U(3,3)/cosTheta);
        rotation[2] = -atan2(U(1,2)/cosTheta, U(1,1)/cosTheta);
    }
    else {
        rotation[0] = 0;
        if ( fabs(U(1,3) + 1) > 1e-10 ) {
            rotation[2] = -rotation[0] - atan2(U(2,1), U(2,2));
            rotation[1] = -M_PI/2;
        }
        else{
            rotation[2] = -rotation[0] + atan2(U(2,1), U(2,2));
            rotation[1] = M_PI/2;
        }
    }

    rotation[0] = RadianToDegree(rotation[0]);
    rotation[1] = RadianToDegree(rotation[1]);
    rotation[2] = RadianToDegree(rotation[2]);

    // translation is last column of transformation
    translation[0] = GetValue(0,3);
    translation[1] = GetValue(1,3);
    translation[2] = GetValue(2,3);

}

// Getter for matrix values
double Transformation::GetValue(int row, int col) const
{
    if (row < 0 || row > 3 || col < 0 || col > 3) {
        throw Error("Invalid row or column index in Transformation::GetValue", geoml::INDEX_ERROR);
    }

    return m_matrix[row][col];
}

std::ostream& operator<<(std::ostream& os, const Transformation& t)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            os << t.m_matrix[i][j] << "\t";
        }
        os << std::endl;
    }
    return os;
}

Transformation operator*(const Transformation & a, const Transformation & b)
{
    Transformation result = b;
    result.PreMultiply(a);
    return result;
}

} // end namespace geoml
