/*
* Copyright (C) 2015 German Aerospace Center (DLR/SC)
*
* Created: 2015-11-20 Martin Siggel <Martin.Siggel@dlr.de>
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

%include std_string.i
%include std_vector.i

%include geoml_config.h


%include std_shared_ptr.i

#define GEOML_API_EXPORT

#ifdef PYTHONNOCC_LEGACY
#define Handle(ClassName)  Handle_##ClassName
#endif

%{
// includes are required to for compilation
#include<Standard_ErrorHandler.hxx>
#include<Standard_Failure.hxx>
#include <geoml/geoml.h>

namespace
{
PyObject* geomlError_to_PyExc(const geoml::Error& err) {
  PyObject* exc_class = PyExc_RuntimeError;

  int code = err.getCode();
  switch (code) {
  case geoml::INDEX_ERROR:
    exc_class = PyExc_IndexError;
    break;
  case geoml::MATH_ERROR:
    exc_class = PyExc_ArithmeticError;
    break;
  case geoml::NULL_POINTER:
    exc_class = PyExc_ValueError;
    break;
  default:
    exc_class = PyExc_RuntimeError;
  }
  return exc_class;
}
}

%}

%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}

%define %catch_exceptions()

%exception {
    try {
        $action
    }
    catch (geoml::Error& err) {
        PyErr_SetString(geomlError_to_PyExc(err), const_cast<char*>(err.what()));
        SWIG_fail;
    }
    catch(Standard_Failure & err) {
        PyErr_SetString(PyExc_RuntimeError, const_cast<char*>((std::string("OpenCASCADE Error: ") + err.GetMessageString()).c_str()));
        SWIG_fail;
    }
    catch(std::exception & err) {
        PyErr_SetString(PyExc_RuntimeError, const_cast<char*>(err.what()));
        SWIG_fail;
    }
    catch(...) {
        PyErr_SetString(PyExc_RuntimeError, const_cast<char*>("An unknown error occurred!"));
        SWIG_fail;
    }
}

%enddef

%include doc.i
