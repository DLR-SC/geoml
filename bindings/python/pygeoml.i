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
// %import "Message.i"
%{
#include <TColStd_module.hxx> // TopoDS.i ( pythonocc-core/src/SWIG_files/headers/TopoDS_module.hxx )
#include <TCollection_module.hxx> // TopoDS.i
#include <TColgp_module.hxx> // TopoDS.i
#include <Geom_module.hxx> // Geom.i ( /src/SWIG_files/headers/Geom_module.hxx )
#include <TopoDS_Compound.hxx> // TopoDS.i ( pythonocc-core/src/SWIG_files/headers/TopoDS_module.hxx ) 
#include <TopoDS.hxx>  // TopoDS.i ( pythonocc-core/src/SWIG_files/headers/TopoDS_module.hxx )
#include <TopoDS_CompSolid.hxx> // TopoDS.i ( pythonocc-core/src/SWIG_files/headers/TopoDS_module.hxx )
#include <TopoDS_Solid.hxx> // TopoDS.i ( pythonocc-core/src/SWIG_files/headers/TopoDS_module.hxx )
#include <TopoDS_Shell.hxx> // TopoDS.i ( pythonocc-core/src/SWIG_files/headers/TopoDS_module.hxx )
#include <TopoDS_Wire.hxx> // TopoDS.i ( pythonocc-core/src/SWIG_files/headers/TopoDS_module.hxx )
#include <TopoDS_Edge.hxx> // TopoDS.i ( pythonocc-core/src/SWIG_files/headers/TopoDS_module.hxx )
#include <TopoDS_Vertex.hxx> // TopoDS.i ( pythonocc-core/src/SWIG_files/headers/TopoDS_module.hxx )
#include <TopAbs_ShapeEnum.hxx> // TopAbs.i  ( src/SWIG_files/headers/TopAbs_module.hxx )
#include <Message_Alert.hxx>
#include <Message_Attribute.hxx>
#include <Message_Printer.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_Report.hxx>
#include <Message_AttributeStream.hxx>
#include <Message_PrinterOStream.hxx>
#include <Message_PrinterSystemLog.hxx>
#include <Message_PrinterToReport.hxx>
#include <TopoDS_AlertWithShape.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_AttributeObject.hxx>
#include <TopoDS_AlertAttribute.hxx>
#include <Message_Algorithm.hxx>
%}


%import "TopoDS.i"
%import "Geom.i" // nur das war drin
// %import "TopAbs.i"


%{
#include <functional>
#include <geoml/geoml.h>
#include "geoml/surfaces/surfaces.h"
#include <geoml/data_structures/Array2d.h>
#include "geoml/curves/curves.h"
#include "geoml/Continuity.h"
#include "geoml/curves/BlendCurve.h"
#include "geoml/geom_topo_conversions/geom_topo_conversions.h"
#include "geoml/naming_choosing/Shape.h"
#include "geoml/boolean_ops/modeling.hpp"
// #include "geoml/predicates/temp_predicates.h"
#include "geoml/predicates/predicate_functions.h"
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
%extend geoml::Shape {
    geoml::Shape select_subshapes(const geoml::ShapePredicate& pred,
                                  int max_depth = std::numeric_limits<int>::max()) const {
        return $self->select_subshapes(pred, max_depth);
    }
}

// %include "TColStd_module.hxx"
// %include "TCollection_module.hxx"
// %include "TColgp_module.hxx"
// %include "Geom_module.hxx"
// %include "TopoDS_Compound.hxx"
// %include "TopoDS.hxx"
// %include "TopoDS_CompSolid.hxx"
// %include "TopoDS_Solid.hxx"
// %include "TopoDS_Shell.hxx"
// %include "TopoDS_Wire.hxx"
// %include "TopoDS_Edge.hxx"
// %include "TopoDS_Vertex.hxx"
// %include "TopAbs_ShapeEnum.hxx"

%include "geoml/geoml.h"
%include "geoml/surfaces/surfaces.h"
%include "geoml/curves/curves.h"
%include "geoml/Continuity.h"
%include "geoml/curves/BlendCurve.h"
%include "geoml/geom_topo_conversions/geom_topo_conversions.h"
%include "geoml/naming_choosing/Shape.h"
%include "geoml/boolean_ops/modeling.hpp"
// %include "geoml/predicates/temp_predicates.h"
%include "geoml/predicates/predicate_functions.h"

