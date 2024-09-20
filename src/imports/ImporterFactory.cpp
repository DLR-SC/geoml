/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-05-08 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "ImporterFactory.h"

#include <algorithm>

namespace geoml
{

ImporterFactory& ImporterFactory::Instance()
{
    static ImporterFactory factory;
    return factory;
}

void ImporterFactory::RegisterImporter(ICADImporterCreator* importerCreator)
{
    if (importerCreator) {
        // build an importer to get supported file type
        auto importer = importerCreator->create();
        if (importer) {
            std::string filetype = importer->SupportedFileType();
            _importerBuilders[filetype] = importerCreator;
        }
    }
}

std::unique_ptr<ICADImporter> ImporterFactory::Create(const std::string &filetype) const
{
    // make the requested filetype to lowercase
    std::string fileTypeToLower = filetype;
    std::transform(fileTypeToLower.begin(), fileTypeToLower.end(), fileTypeToLower.begin(), ::tolower);

    ImporterMap::const_iterator it = _importerBuilders.find(fileTypeToLower);
    if (it != _importerBuilders.end()) {
        ICADImporterCreator* creator = it->second;
        return creator->create();
    }
    else {
        return {};
    }
}

bool ImporterFactory::ImporterSupported(const std::string& filetype) const
{
    // make the requested filetype to lowercase
    std::string fileTypeToLower = filetype;
    std::transform(fileTypeToLower.begin(), fileTypeToLower.end(), fileTypeToLower.begin(), ::tolower);

    ImporterMap::const_iterator it = _importerBuilders.find(fileTypeToLower);
    if (it != _importerBuilders.end()) {
        return true;
    }
    else {
        return false;
    }
}

}


