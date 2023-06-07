/* 
 * Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
 *
 * Created: 2013-02-13 Markus Litz <Markus.Litz@dlr.de>

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
/**
 * @file
 * @brief  Implementation of the geoml Logger.
 */

#ifndef LOGGING_H
#define LOGGING_H

#include "geoml_config.h"
#include "geoml_internal.h"

#include <string>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <memory>



namespace geoml 
{
    enum LogLevel
    {
        LogLevel_SILENT,
        LogLevel_ERROR,
        LogLevel_WARNING,
        LogLevel_INFO,
        LogLevel_DEBUG,
        LogLevel_DEBUG1,
        LogLevel_DEBUG2,
        LogLevel_DEBUG3,
        LogLevel_DEBUG4
    };

    #ifndef LOG_MAX_LEVEL
      #define LOG_MAX_LEVEL geoml::LogLevel_DEBUG4
    #endif
    #ifndef LOG_MIN_LEVEL
        #define LOG_MIN_LEVEL geoml::LogLevel_SILENT
    #endif

    /**
     * This macros can be used like streams e.g.: LOG(ERROR) << "that is an error";
     * Following log levels are supported: ERROR, WARNING, INFO, DEBUG, DEBUG1, DEBUG2, DEBUG3, DEBUG4
     */
    #define LOG(level) \
    if (geoml::LogLevel_ ## level > LOG_MAX_LEVEL || geoml::LogLevel_ ## level <= LOG_MIN_LEVEL) ;\
    else geoml::DummyLogger_().AppendToStream(geoml::LogLevel_ ## level, __FILE__, __LINE__)

    #define DLOG(level) \
    if (geoml::LogLevel_ ## level > LOG_MAX_LEVEL) ;\
    else geoml::DebugStream_().AppendToStream(geoml::LogLevel_ ## level, __FILE__, __LINE__)

    class DummyLogger_
    {
    public:
        GEOML_EXPORT DummyLogger_();
        GEOML_EXPORT virtual ~DummyLogger_();
        GEOML_EXPORT std::ostringstream& AppendToStream(LogLevel level, const char *file, int line);
    protected:
        std::ostringstream stream;
    private:
        DummyLogger_(const DummyLogger_&);
        DummyLogger_& operator =(const DummyLogger_&);
        
        LogLevel _lastLevel;
    };

    class DebugStream_
    {
    public:
        GEOML_EXPORT DebugStream_();
        GEOML_EXPORT virtual ~DebugStream_();
        GEOML_EXPORT std::ostringstream& AppendToStream(LogLevel level, const char *file, int line);
    protected:
        std::ostringstream stream;
    private:
        DebugStream_(const DebugStream_&);
        DebugStream_& operator =(const DebugStream_&);

        LogLevel _lastLevel;
    };

// define smart pointer to logger
class ILogger;
typedef class std::shared_ptr<ILogger> PLogger;

class Logging
{
public:
    // Returns a reference to the only instance of this class
    GEOML_EXPORT static Logging& Instance();
    
    // convenience functions that insert the appropriate loggers
    GEOML_EXPORT void LogToFile(const char* filePrefix);
    GEOML_EXPORT void LogToStream(FILE * fp);
    GEOML_EXPORT void SetLogFileEnding(const char* ending);
    GEOML_EXPORT void SetTimeIdInFilenameEnabled(bool enabled);
    GEOML_EXPORT void LogToConsole();
    GEOML_EXPORT void SetConsoleVerbosity(LogLevel vlevel);
    
    // allows installing a custom log sink/receiver
    // The logger becomes property of this class
    // Therefore the logger should not be deleted
    // manually.
    GEOML_EXPORT void SetLogger(PLogger);
    GEOML_EXPORT PLogger GetLogger();

    // Destructor
    GEOML_EXPORT ~Logging();

private:
    // Constructor
    Logging();

    // Copy constructor
    Logging(const Logging& )                { /* Do nothing */ }

    // Assignment operator
    void operator=(const Logging& )             { /* Do nothing */ }
    
    PLogger      _myLogger;
    std::string        _fileEnding;
    bool               _timeIdInFilename;
    LogLevel       _consoleVerbosity;
    PLogger      _consoleLogger;

};

// get log level string (for testing purposes)
GEOML_EXPORT std::string getLogLevelString(LogLevel level);

} // end namespace geoml

#endif // LOGGING_H
