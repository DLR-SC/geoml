/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2013-10-26 Martin Siggel <Martin.Siggel@dlr.de>
*/

#ifndef ILOGGER_H
#define ILOGGER_H
#include "logging/Logging.h"

namespace geoml 
{

class ILogger
{
public:
    virtual ~ILogger() {}
    virtual void LogMessage(LogLevel, const char * message) = 0;
    virtual void SetVerbosity(LogLevel) = 0;
};

}

#endif // ILOGGER_H
