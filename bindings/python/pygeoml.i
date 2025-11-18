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
#include <TopAbs_ShapeEnum.hxx> 
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
%import "Geom.i" 


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
#include "geoml/predicates/predicate_functions.h"
#include "geoml/boolean_ops/modeling.hpp"
%} 


%feature("autodoc", "3");


%catch_exceptions()

%include "geoml/data_structures/Array2d.h"
%include "geoml/naming_choosing/Shape.h"

%template(CurveList) std::vector<Handle(Geom_Curve)>;
%template(CPointContainer) std::vector<gp_Pnt>;
%template(StandardRealList) std::vector<Standard_Real>;
%template(IntList) std::vector<int>;
%template(Array2dStandardReal) geoml::Array2d<Standard_Real>;
%template(Array2dgp_Pnt) geoml::Array2d<gp_Pnt>;
%template(add_persistent_meta_tag_to_subshapes) geoml::add_persistent_meta_tag_to_subshapes<geoml::ShapePredicate>;

// %ignore Array2dStandardReal::at;

%extend geoml::Array2d<Standard_Real> {
    double my_at(int row, int col) {
        return $self->at(row, col);        
    }
}

%extend geoml::Shape {
    geoml::Shape select_subshapes(const geoml::ShapePredicate& pred,
                                  int max_depth = std::numeric_limits<int>::max()) const {
        return $self->select_subshapes(pred, max_depth);
    }

    geoml::Shape filter(const geoml::ShapePredicate& pred) const {
        return $self->filter(pred);  
    }

    void add_meta_tag_to_subshapes(const geoml::ShapePredicate& pred, std::string const& input_tag) {
        $self->add_meta_tag_to_subshapes(pred, input_tag);        
    }

    Shape& __getitem__(int i) {
        return $self->operator[](i);
    }
    Shape __sub__(Shape const& other) {
        return geoml::operator-(*$self, other);
    }
    Shape __add__(Shape const& other) {
        return geoml::operator+(*$self, other);
    }
}
%extend geoml::TagTrack {
    TagTrack(std::string const& tag,
             geoml::ShapePredicate const& criterion,
             int remaining_steps) {
        return new geoml::TagTrack(tag, criterion, remaining_steps);
    }
}
%extend geoml::ShapePredicate {
    // Map operator! to Python's __invert__ (i.e. ~)
    ShapePredicate __invert__() {
        return operator!(*$self);
    }
    // Map operator&& to Python’s __and__ (i.e. &)
    ShapePredicate __and__(ShapePredicate const& other) {
        return operator&&(*$self, other);
    }

    // Map operator|| to Python’s __or__ (i.e. |)
    ShapePredicate __or__(ShapePredicate const& other) {
        return operator||(*$self, other);
    }
}

%include "geoml/geoml.h"
%include "geoml/surfaces/surfaces.h"
%include "geoml/curves/curves.h"
%include "geoml/Continuity.h"
%include "geoml/curves/BlendCurve.h"
%include "geoml/geom_topo_conversions/geom_topo_conversions.h"
%include "geoml/naming_choosing/Shape.h"
%include "geoml/boolean_ops/modeling.hpp"
%include "geoml/predicates/predicate_functions.h"
%include "geoml/boolean_ops/modeling.hpp"

