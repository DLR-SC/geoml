/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-10-26 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include "geoml_internal.h"
#include "ILogger.h"

namespace geoml 
{

class ConsoleLogger : public ILogger
{
public:
    GEOML_EXPORT ConsoleLogger();
    GEOML_EXPORT ~ConsoleLogger() override {}
    GEOML_EXPORT void LogMessage(LogLevel, const char * message) override;
    GEOML_EXPORT void SetVerbosity(LogLevel) override;
private:
    LogLevel verbosity;
};

}

#endif // CONSOLELOGGER_H
