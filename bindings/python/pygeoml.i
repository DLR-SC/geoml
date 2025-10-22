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
#include <TopoDS_Compound.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
%}

%import Geom.i

%{
#include <geoml/geoml.h>
#include "geoml/surfaces/surfaces.h"
#include <geoml/data_structures/Array2d.h>
#include "geoml/curves/curves.h"
#include "geoml/Continuity.h"
#include "geoml/curves/BlendCurve.h"
#include "geoml/geom_topo_conversions/geom_topo_conversions.h"
%}

%feature("autodoc", "3");

%catch_exceptions()

%include "geoml/data_structures/Array2d.h"

%template(CurveList) std::vector<Handle(Geom_Curve)>;
%template(CPointContainer) std::vector<gp_Pnt>;
%template(StandardRealList) std::vector<Standard_Real>;
%template(IntList) std::vector<int>;
%template(Array2dStandardReal) geoml::Array2d<Standard_Real>;
%template(Array2dgp_Pnt) geoml::Array2d<gp_Pnt>;

%include "TColStd_module.hxx"
%include "TCollection_module.hxx"
%include "TColgp_module.hxx"
%include "Geom_module.hxx"
%include "TopoDS_Compound.hxx"
%include "TopoDS.hxx"
%include "TopoDS_CompSolid.hxx"
%include "TopoDS_Solid.hxx"
%include "TopoDS_Shell.hxx"
%include "TopoDS_Wire.hxx"
%include "TopoDS_Edge.hxx"
%include "TopoDS_Vertex.hxx"

%include "geoml/geoml.h"
%include "geoml/surfaces/surfaces.h"
%include "geoml/curves/curves.h"
%include "geoml/Continuity.h"
%include "geoml/curves/BlendCurve.h"
%include "geoml/geom_topo_conversions/geom_topo_conversions.h"

