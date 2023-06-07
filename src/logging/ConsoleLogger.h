/* 
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2013-10-26 Martin Siggel <Martin.Siggel@dlr.de>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
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
