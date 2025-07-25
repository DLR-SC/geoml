/*
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2013-10-29 Martin Siggel <Martin.Siggel@dlr.de>
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

#include "FileLogger.h"
#include "geoml/error.h"

#include <cstdio>
#include <ctime>
#include <iostream>

namespace geoml 
{

FileLogger::FileLogger(FILE * file) : logFileStream(file), verbosity(LogLevel_DEBUG4)
{
    if (!logFileStream) {
        throw Error("Null pointer for argument file in FileLogger", geoml::NULL_POINTER);
    }
    
    fileOpened = false;
}

FileLogger::FileLogger(const char* filename) : verbosity(LogLevel_DEBUG4)
{
    logFileStream = fopen(filename,"w");
    if (!logFileStream) {
        throw Error("Log file can not be created FileLogger", geoml::OPEN_FAILED);
    }
    fileOpened = true;

    // timestamp
    time_t rawtime;
    time (&rawtime);
    tm *timeinfo = localtime (&rawtime);
    char buffer [160];
    strftime (buffer,160,"geoml log file created at: %y/%m/%d %H:%M:%S",timeinfo);
    
    fprintf(logFileStream, "%s\n", buffer);
}

FileLogger::~FileLogger()
{
    if (fileOpened) {
        fclose(logFileStream);
    }
}

void FileLogger::LogMessage(LogLevel level, const char *message)
{
    if (level<=verbosity) {
        if (logFileStream) {
            const std::lock_guard<std::mutex> lock(mutex);
            fprintf(logFileStream, "%s\n", message);
        }
    }
}
void FileLogger::SetVerbosity(LogLevel vlevel)
{
    verbosity=vlevel;
}

} // namespace geoml

