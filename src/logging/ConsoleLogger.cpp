/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-10-26 Martin Siggel <Martin.Siggel@dlr.de>
*/

#include "ConsoleLogger.h"

#include <iostream>

namespace geoml
{

ConsoleLogger::ConsoleLogger() : verbosity(LogLevel_WARNING)
{                                            
}

void ConsoleLogger::LogMessage(LogLevel level, const char *message)
{
    if (level<=verbosity) {
        if (level == LogLevel_ERROR || level == LogLevel_WARNING) {
            fprintf(stderr, "%s\n", message);
        }
        else {
            fprintf(stdout, "%s\n", message);
        }
    }
}

void ConsoleLogger::SetVerbosity(LogLevel vlevel)
{
    verbosity=vlevel;
}

} // end namespace geoml
