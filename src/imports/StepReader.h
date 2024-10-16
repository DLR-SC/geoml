/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-05-05 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef STEPREADER_H
#define STEPREADER_H

#include "geoml_internal.h"
#include "geoml_config.h"
#include "ICADImporter.h"

#include <string>

namespace geoml
{

class ImportStep : public ICADImporter
{
public:
    GEOML_EXPORT ImportStep();
    
    /// reads in a step file
    GEOML_EXPORT ListPNamedShape Read(const std::string stepFileName) override;

    GEOML_EXPORT std::string SupportedFileType() const override;
    
    GEOML_EXPORT ~ImportStep();
};

}

#endif // STEPREADER_H
