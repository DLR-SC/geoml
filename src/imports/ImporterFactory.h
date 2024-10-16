/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-05-08 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef IMPORTERFACTORY_H
#define IMPORTERFACTORY_H

#include "geoml_internal.h"

#include "ICADImporter.h"
#include "ICADImporterCreator.h"

#include <map>

namespace geoml
{

class ImporterFactory
{
public:
    GEOML_EXPORT static ImporterFactory& Instance();

    /// Registers a CAD Importer at the factory
    GEOML_EXPORT void RegisterImporter(ICADImporterCreator* creator);

    /// Creates an importer for the matching file format
    /// Returns NULL, if the format is unknown
    GEOML_EXPORT std::unique_ptr<ICADImporter> Create(const std::string& filetype) const;
   
    /// Returns true, if an importer was registered for the specified file type
    GEOML_EXPORT bool ImporterSupported(const std::string& filetype) const;

private:
    ImporterFactory() = default;

    typedef std::map<std::string, ICADImporterCreator*>  ImporterMap;
    ImporterMap _importerBuilders;

};

}

#endif // IMPORTERFACTORY_H
