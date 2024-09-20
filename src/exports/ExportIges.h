/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2010-08-13 Markus Litz <Markus.Litz@dlr.de>
*/
/**
* @file
* @brief  Export routines for CPACS configurations.
*/

#ifndef EXPORTIGES_H
#define EXPORTIGES_H

#include "geoml_config.h"
#include "geoml_internal.h"
#include "CADExporter.h"


class IGESControl_Writer;

namespace geoml 
{

class CCPACSConfiguration;

enum FaceNameSettings
{
    UIDOnly,
    FaceNameOnly,
    UIDandFaceName,
    None
};

class IgesOptions : public ExporterOptions
{
public:
    IgesOptions()
    {
        Set("ShapeGroupMode", NAMED_COMPOUNDS);
        AddOption("IGES5.3", true);
        AddOption("FaceNames", FaceNameOnly);
    }
};

class IgesShapeOptions : public ShapeExportOptions
{
public:
    IgesShapeOptions()
    {
        AddOption("Layer", -1);
    }

    IgesShapeOptions(int layer)
    {
        AddOption("Layer", layer);
    }

    void SetLayer(int layer)
    {
        Set("Layer", layer);
    }
};

class ExportIges : public CADExporter
{

public:
    // Constructor
    GEOML_EXPORT ExportIges(const ExporterOptions& = DefaultExporterOption());

    GEOML_EXPORT ExporterOptions GetDefaultOptions() const override;
    GEOML_EXPORT ShapeExportOptions GetDefaultShapeOptions() const override;

private:
    // Actual implementation of the IGES file writing
    GEOML_EXPORT bool WriteImpl(const std::string& filename) const override;

    std::string SupportedFileTypeImpl() const override
    {
        return "igs;iges";
    }

    // Assignment operator
    void operator=(const ExportIges& );
    void AddToIges(PNamedShape shape, IGESControl_Writer& writer, int level = 0) const;

    void SetTranslationParameters() const;
};

template <>
inline void from_string<FaceNameSettings>(const std::string &s, FaceNameSettings &t)
{
    std::string value = geoml::to_upper(s);
    if (value == "UIDONLY") {
        t = UIDOnly;
    }
    else if (value == "FACENAMEONLY") {
        t = FaceNameOnly;
    }
    else if (value == "UIDANDFACENAME") {
        t = UIDandFaceName;
    }
    else if (value == "NONE") {
        t = None;
    }
    else {
        throw Error("Cannot convert string to FaceNameSettings");
    }
}

} // end namespace geoml

#endif // EXPORTIGES_H
