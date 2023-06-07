/*
* Copyright (C) 2015 German Aerospace Center (DLR/SC)
*
* Created: 2015-11-23 Martin Siggel <Martin.Siggel@dlr.de>
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

%module (package="geoml") pygeoml

%include common.i

%{
#include <TColStd_module.hxx>
#include <TCollection_module.hxx>
#include <TColgp_module.hxx>
#include <Geom_module.hxx>
%}

%import Geom.i

%{
#include <geoml/geoml.h>
#include <geoml/surface/modeling.h>
%}

%feature("autodoc", "3");

%catch_exceptions()

%template(CurveList) std::vector<Handle(Geom_Curve)>;

%include "geoml/geoml.h"
%include "geoml/surface/modeling.h"


