/*
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2013-11-11 Martin Siggel <Martin.Siggel@dlr.de>
*
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

#ifndef CNAMEDSHAPE_H
#define CNAMEDSHAPE_H

#include "geoml_internal.h"
#include "CSharedPtr.h"
#include "PNamedShape.h"
#include "geometry/Transformation.h"

#include <string>
#include <vector>
#include <TopoDS_Shape.hxx>

class CFaceTraits;

typedef std::vector<std::string> StringList;
typedef std::vector<CFaceTraits> FaceList;

/**
 * @brief The CFaceTraits class stores face metadata like
 * a reference to the shape where the face was originally created
 * in and its name.
 */
class CFaceTraits
{
public:
    GEOML_EXPORT CFaceTraits();
    
    GEOML_EXPORT unsigned int Index() const;
    GEOML_EXPORT void SetIndex(unsigned int);
    
    GEOML_EXPORT const PNamedShape Origin() const;
    GEOML_EXPORT void SetOrigin(const PNamedShape);

    // marks this face trait as derived from another shapes face, where
    // origin is the other shape and iface is the face index in the original shape
    GEOML_EXPORT void SetDerivedFromShape(PNamedShape origin, unsigned int iface);
    
    GEOML_EXPORT std::string Name() const;
    GEOML_EXPORT void SetName(const std::string&);
    
    GEOML_EXPORT void SetComponentUID(const std::string& uid);
    GEOML_EXPORT std::string ComponentUID() const;
    
    GEOML_EXPORT void SetTransformation(const geoml::Transformation& trafo);
    GEOML_EXPORT const geoml::Transformation& Transformation() const;
    
private:
    PNamedShape  _origin;        /** Pointer to the original shape where this face was created */
    unsigned int _indexInOrigin; /** Index of face in original shape */
    std::string   _faceName;     /** Name of the face */
    std::string _componentUID;   /** UID of a parent object the face belongs to */
    
    geoml::Transformation _localTransformation; /** A transformation property, e.g. for faces on flaps */
};

/**
 * @brief The CNamedShape class stores a TopoDS_Shape and additional
 * metadata to origin of faces, names and history
 */
class CNamedShape
{
public:
    GEOML_EXPORT CNamedShape();
    GEOML_EXPORT CNamedShape(const TopoDS_Shape& shape, const std::string& shapeName);
    GEOML_EXPORT CNamedShape(const TopoDS_Shape& shape, const std::string &shapeName, const std::string &shapeShortName);
    GEOML_EXPORT CNamedShape(const CNamedShape&);
    GEOML_EXPORT CNamedShape &operator= (const CNamedShape&);
    GEOML_EXPORT ~CNamedShape();

    // creates a true copy, i.e. it duplicates the underlying shape
    GEOML_EXPORT PNamedShape DeepCopy() const;

    GEOML_EXPORT void Clear();

    // getters
    GEOML_EXPORT const TopoDS_Shape& Shape() const;
    GEOML_EXPORT std::string Name()  const;
    GEOML_EXPORT std::string ShortName() const;

    GEOML_EXPORT unsigned int GetFaceCount() const;
    GEOML_EXPORT const CFaceTraits& GetFaceTraits(unsigned int iFace) const;
    GEOML_EXPORT CFaceTraits& FaceTraits(unsigned int iFace);

    // setters
    GEOML_EXPORT void SetShape(const TopoDS_Shape&);
    GEOML_EXPORT void SetName(const std::string&);
    GEOML_EXPORT void SetShortName(const std::string &);
    GEOML_EXPORT void SetFaceTraits(int iFace, const CFaceTraits& traits);

protected:
    void InitFaceTraits();

    TopoDS_Shape  _myshape;
    std::string   _myname;
    std::string   _myshortName;
    FaceList      _myfaceTraits;
};

#endif // CNAMEDSHAPE_H
