/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-10-29 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "LogSplitter.h"

namespace geoml 
{

LogSplitter::LogSplitter() : verbosity(LogLevel_DEBUG4)
{
}

LogSplitter::~LogSplitter()
{
}


void LogSplitter::AddLogger(PLogger logger)
{
    if (logger) {
        _loggers.push_back(logger);
    }
}

// override from ILogger
void LogSplitter::LogMessage(LogLevel level, const char * message) 
{
    if (level<=verbosity) {
        for (std::vector<PLogger>::iterator it = _loggers.begin(); it != _loggers.end(); ++it) {
            if (!*it) {
                continue;
            }
            (*it)->LogMessage(level, message);
        }
    }
}

void LogSplitter::SetVerbosity(LogLevel vlevel)
{
    verbosity=vlevel;
}

} // namespace geoml

