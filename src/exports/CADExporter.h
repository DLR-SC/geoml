/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2016 German Aerospace Center (DLR)
*
* Created: 2016-12-21 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef CADEXPORTER_H
#define CADEXPORTER_H

#include "geoml_internal.h"
#include "PNamedShape.h"
#include "ListPNamedShape.h"
#include "COptionList.h"
#include "CCPACSImportExport.h"

namespace geoml
{

class CCPACSConfiguration;


class ExporterOptions : public COptionList
{
public:
    ExporterOptions()
    {
        AddOption("IsDefault", false);
        AddOption("ShapeGroupMode", WHOLE_SHAPE);
    }

    void SetApplySymmetries(bool apply)
    {
        Set("ApplySymmetries", apply);
    }

    void SetIncludeFarfield(bool include)
    {
        Set("IncludeFarfield", include);
    }

    void SetShapeGroupMode(geoml::ShapeGroupMode mode)
    {
        Set("ShapeGroupMode", mode);
    }

    ShapeGroupMode GroupMode() const
    {
        return Get<ShapeGroupMode>("ShapeGroupMode");
    }

    bool IsDefault() const
    {
        return Get<bool>("IsDefault");
    }
};

class DefaultExporterOption : public ExporterOptions
{
public:
    DefaultExporterOption()
    {
        Set("IsDefault", true);
    }
};

class ShapeExportOptions : public COptionList
{
public:
    ShapeExportOptions()
    {
        AddOption("IsDefault", false);
    }

    bool IsDefault() const
    {
        return Get<bool>("IsDefault");
    }
};

class DefaultShapeExportOptions : public ShapeExportOptions
{
public:
    DefaultShapeExportOptions()
    {
        Set("IsDefault", true);
    }
};

class TriangulatedExportOptions : public ShapeExportOptions
{
public:
    TriangulatedExportOptions()
    {
        AddOption("Deflection", 0.001);
    }

    TriangulatedExportOptions(double deflection)
    {
        AddOption("Deflection", deflection);
    }
};

/**
 * @brief Abstract base class for CAD exports
 */
class CADExporter
{
public:
    /// Constructor
    GEOML_EXPORT CADExporter(const ExporterOptions& options = DefaultExporterOption());

    // Empty destructor
    GEOML_EXPORT virtual ~CADExporter() { /* empty */}

    virtual ExporterOptions GetDefaultOptions() const = 0;
    virtual ShapeExportOptions GetDefaultShapeOptions() const = 0;

    /// Adds a shape
    GEOML_EXPORT void AddShape(PNamedShape shape, const ShapeExportOptions& options = DefaultShapeExportOptions());


    GEOML_EXPORT bool Write(const std::string& filename) const;

    /// Number of shapes
    GEOML_EXPORT size_t NShapes() const;

    /// Returns all shapes added to the exporter
    GEOML_EXPORT PNamedShape GetShape(size_t iShape) const;

    GEOML_EXPORT const ShapeExportOptions& GetOptions(size_t iShape) const;
    GEOML_EXPORT const ExporterOptions& GlobalExportOptions() const;

    GEOML_EXPORT std::string SupportedFileType() const;


private:
    /// must be overridden by the concrete implementation
    virtual bool WriteImpl(const std::string& filename) const = 0;
    
    /// must be overridden. If multiple types supported, separate with a ";"
    virtual std::string SupportedFileTypeImpl() const = 0;

    ListPNamedShape _shapes;
    mutable ExporterOptions _globalOptions;
    std::vector<ShapeExportOptions> _shapeOptions;

};

class ICADExporterBuilder
{
public:
    GEOML_EXPORT virtual std::unique_ptr<CADExporter> create(const ExporterOptions& options = DefaultExporterOption()) const = 0;
    GEOML_EXPORT virtual ~ICADExporterBuilder(){}
};

template <class T>
class CCADExporterBuilder : public ICADExporterBuilder
{
public:
   GEOML_EXPORT  std::unique_ptr<CADExporter> create(const ExporterOptions& options = DefaultExporterOption()) const override
   {
        return std::make_unique<T>(options);
   }
};

} // namespace geoml

#endif // CADEXPORTER_H
