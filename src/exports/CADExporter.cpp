/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2016 German Aerospace Center (DLR)
*
* Created: 2016-12-21 Martin Siggel <Martin.Siggel@dlr.de>
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
