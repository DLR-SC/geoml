/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2015 German Aerospace Center (DLR)
*
* Created: 2015-05-27 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef ICADIMPORTER_H
#define ICADIMPORTER_H

#include "ListPNamedShape.h"

#include <string>

namespace geoml
{

/**
 * @brief The ICADImporter Interface class should be common
 * for all CAD importing implementations
 */
class ICADImporter
{
public:
    /// read in file name
    virtual ListPNamedShape Read(const std::string fileName) = 0;

    /// should return the supported file type, e.g. "step"
    virtual std::string SupportedFileType() const  = 0;

    virtual ~ICADImporter() {}

};

}

#endif // ICADIMPORTER_H
