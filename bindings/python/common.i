/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-11-20 Martin Siggel <Martin.Siggel@dlr.de>
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
