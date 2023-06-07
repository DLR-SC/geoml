/* 
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2013-02-28 Markus Litz <Markus.Litz@dlr.de>
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
* @brief  STEP-Export routines for CPACS configurations.
*/

#ifndef EXPORTSTEP_H
#define EXPORTSTEP_H

#include "geoml_internal.h"
#include "ListPNamedShape.h"
#include "CADExporter.h"

class STEPControl_Writer;

namespace geoml 
{

class CCPACSConfiguration;

class StepOptions : public ExporterOptions
{
public:
    StepOptions()
    {
        Set("ShapeGroupMode", NAMED_COMPOUNDS);
    }
};

class StepShapeOptions : public ShapeExportOptions
{
public:
    StepShapeOptions(){}
};

class ExportStep : public CADExporter
{

public:
    // Constructor
    GEOML_EXPORT ExportStep(const ExporterOptions& opt = DefaultExporterOption());

    GEOML_EXPORT ExporterOptions GetDefaultOptions() const override;
    GEOML_EXPORT ShapeExportOptions GetDefaultShapeOptions() const override;

private:
    // Writes the step file
    GEOML_EXPORT bool WriteImpl(const std::string& filename) const override;

    std::string SupportedFileTypeImpl() const override
    {
        return "step;stp";
    }

    // Assignment operator
    void operator=(const ExportStep& ) { /* Do nothing */ }

    void AddToStep(PNamedShape shape, STEPControl_Writer &writer) const;
};

} // end namespace geoml

#endif // EXPORTSTEP_H
