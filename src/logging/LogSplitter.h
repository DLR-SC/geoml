/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-10-29 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef LOGSPLITTER_H
#define LOGSPLITTER_H

#include "geoml_internal.h"
#include "ILogger.h"
#include "Logging.h"
#include <vector>

namespace geoml 
{

class LogSplitter : public ILogger
{
public:
    GEOML_EXPORT LogSplitter();
    GEOML_EXPORT ~LogSplitter() override;

    // Adds a logger to the splitter
    GEOML_EXPORT void AddLogger(PLogger);

    // override from ILogger
    GEOML_EXPORT void LogMessage(LogLevel, const char * message) override;
    GEOML_EXPORT void SetVerbosity(LogLevel) override;

private:
    std::vector<PLogger> _loggers;
    LogLevel verbosity;
};

} // namespace geoml

#endif // LOGSPLITTER_H
