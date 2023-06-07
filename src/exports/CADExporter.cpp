/*
* Copyright (C) 2016 German Aerospace Center (DLR/SC)
*
* Created: 2016-12-21 Martin Siggel <Martin.Siggel@dlr.de>
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

#include "CADExporter.h"

#include <string>
#include <algorithm>
#include <cassert>


namespace geoml
{

bool CADExporter::Write(const std::string &filename) const
{
    return WriteImpl(filename);
}

CADExporter::CADExporter(const ExporterOptions &options)
    : _globalOptions(options)
{
}

void CADExporter::AddShape(PNamedShape shape, const ShapeExportOptions& options)
{
    if (shape) {
        _shapes.push_back(shape);
        _shapeOptions.push_back(options.Get<bool>("IsDefault") ? GetDefaultShapeOptions() : options);
    }
}

size_t CADExporter::NShapes() const
{
    return _shapes.size();
}

PNamedShape CADExporter::GetShape(size_t iShape) const
{
    return _shapes.at(iShape);
}

const ShapeExportOptions& CADExporter::GetOptions(size_t iShape) const
{
    return _shapeOptions.at(iShape);
}

const ExporterOptions &CADExporter::GlobalExportOptions() const
{
    if (_globalOptions.IsDefault()) {
        _globalOptions = GetDefaultOptions();
    }
    return _globalOptions;
}


std::string CADExporter::SupportedFileType() const
{
    return SupportedFileTypeImpl();
}

} // namespace geoml
