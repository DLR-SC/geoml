/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-10-29 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef FILELOGGER_H
#define FILELOGGER_H

#include "geoml_internal.h"
#include "ILogger.h"
#include <cstdio>
#include <mutex>

namespace geoml 
{

class FileLogger : public ILogger
{
public:
    GEOML_EXPORT FileLogger(const char* filename);
    GEOML_EXPORT FileLogger(FILE * file);
    GEOML_EXPORT ~FileLogger() override;
    
    GEOML_EXPORT void LogMessage(LogLevel, const char * message) override;
    GEOML_EXPORT void SetVerbosity(LogLevel) override;

private:
    FILE * logFileStream;
    bool fileOpened;
    std::mutex mutex;
    LogLevel verbosity;
};

} // namespace geoml

#endif // FILELOGGER_H
