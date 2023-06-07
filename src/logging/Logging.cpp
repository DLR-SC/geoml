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
* @brief  Implementation of a simple GEOML Logger.
*/

#include "Logging.h"
#include "ILogger.h"
#include "FileLogger.h"
#include "LogSplitter.h"
#include "ConsoleLogger.h"
#include "CSharedPtr.h"
#include <ctime>
#include <cstring>
#include <string>

//macro that extracts the filename of the current file
#if defined _WIN32 || defined __WIN32__
#define BASENAME(MYFILE) (strrchr((MYFILE), '\\') ? strrchr((MYFILE), '\\') + 1 : (MYFILE))
#else
#define BASENAME(MYFILE) (strrchr((MYFILE), '/') ? strrchr((MYFILE), '/') + 1 : (MYFILE))
#endif

static const char* const LogLevelStrings[] = {"SLT", "ERR", "WRN", "INF", "DBG", "DBG1", "DBG2", "DBG3", "DBG4"};

namespace geoml 
{


Logging::Logging()
{
    _fileEnding = "log";
    _timeIdInFilename = true;
    _consoleVerbosity = LogLevel_WARNING;
    // set logger to console logger
    PLogger consoleLogger(new ConsoleLogger);
    consoleLogger->SetVerbosity(_consoleVerbosity);
    SetLogger(consoleLogger);
    // set _consoleLogger variable
    _consoleLogger=_myLogger;
}

Logging::~Logging() = default;

PLogger Logging::GetLogger()
{
    return _myLogger;
}

void Logging::SetLogger(PLogger logger)
{

    _myLogger = logger;
    _consoleLogger.reset();
}

Logging& Logging::Instance()
{
    static Logging instance;
    return instance;
}

void Logging::LogToFile(const char* prefix) 
{
    char buffer[80];
    if (_timeIdInFilename) {
        time_t rawtime;
        time(&rawtime);
        tm *timeinfo = localtime(&rawtime);
        strftime(buffer, 80, "%y%m%d-%H%M%S", timeinfo);
    } else {
        buffer[0] = 0;
    }
    std::string filename = std::string(prefix) + buffer + "." + _fileEnding;
    
    // add file and console logger to splitter
    CSharedPtr<LogSplitter> splitter (new LogSplitter);
    PLogger fileLogger (new FileLogger(filename.c_str()));
    splitter->AddLogger(fileLogger);
    PLogger consoleLogger (new ConsoleLogger);
    consoleLogger->SetVerbosity(_consoleVerbosity);
    splitter->AddLogger(consoleLogger);
    SetLogger(splitter);
    // set _consoleLogger variable
    _consoleLogger=consoleLogger;
}

void Logging::LogToStream(FILE * fp) 
{

    // add file and console logger to splitter
    CSharedPtr<LogSplitter> splitter (new LogSplitter);
    PLogger fileLogger (new FileLogger(fp));
    splitter->AddLogger(fileLogger);
    PLogger consoleLogger (new ConsoleLogger);
    consoleLogger->SetVerbosity(_consoleVerbosity);
    splitter->AddLogger(consoleLogger);
    SetLogger(splitter);
    // set _consoleLogger variable
    _consoleLogger=consoleLogger;
}

void Logging::SetLogFileEnding(const char* ending) 
{
    _fileEnding = ending;
}

void Logging::SetTimeIdInFilenameEnabled(bool enabled) 
{
    _timeIdInFilename = enabled;
}

void Logging::SetConsoleVerbosity(LogLevel vlevel) 
{
    _consoleVerbosity=vlevel;
    if (_consoleLogger) {
        _consoleLogger->SetVerbosity(_consoleVerbosity);
    }
}

void Logging::LogToConsole() 
{
#ifdef GLOG_FOUND
    google::LogToStderr();
#else
    
    PLogger consoleLogger (new ConsoleLogger);
    consoleLogger->SetVerbosity(_consoleVerbosity);
    SetLogger(consoleLogger);
    // set _consoleLogger variable
    _consoleLogger=consoleLogger;
#endif
}

std::string getLogLevelString(LogLevel level)
{
    return LogLevelStrings[(int)level];
}

#ifndef GLOG_FOUND

DummyLogger_::DummyLogger_(){}
DummyLogger_::~DummyLogger_()
{
    geoml::PLogger logger = Logging::Instance().GetLogger();
    std::string msg = stream.str();
    if (msg.size() > 0 && msg[msg.size()-1] == '\n') {
        msg.resize(msg.size() - 1);
    }
    if (logger) {
        logger->LogMessage(_lastLevel, msg.c_str());
    }
    else {
        if (_lastLevel == LogLevel_WARNING || _lastLevel == LogLevel_ERROR ) {
            fprintf(stderr, "%s\n", msg.c_str());
        }
        else {
            fprintf(stdout, "%s\n", msg.c_str());
        }
    }
}


std::ostringstream& DummyLogger_::AppendToStream(LogLevel level, const char* file, int line) 
{
    _lastLevel = level;
    
    stream <<  getLogLevelString(level) << " ";

    // timestamp
    time_t rawtime;
    time (&rawtime);
    struct tm *timeinfo = localtime (&rawtime);
    char buffer [80];
    strftime (buffer,80,"%m/%d %H:%M:%S",timeinfo);
    stream << buffer << " ";

    stream << BASENAME(file) << ":" << line  << "] ";
    stream << std::string(level > LogLevel_DEBUG ? (int)level - (int)LogLevel_DEBUG : 0, '\t');
    return stream;
}

DebugStream_::DebugStream_(){}
DebugStream_::~DebugStream_() 
{
#ifdef DEBUG
    geoml::PLogger logger = Logging::Instance().GetLogger();
    std::string msg = stream.str();
    if (msg.size() > 0 && msg[msg.size()-1] == '\n') {
        msg.resize(msg.size() - 1);
    }
    if (logger) {
        logger->LogMessage(_lastLevel, msg.c_str());
    }
    else {
        if (_lastLevel == geoml::LogLevel_WARNING || _lastLevel == geoml::LogLevel_ERROR) {
            fprintf(stderr, "%s\n", msg.c_str());
        }
        else {
            fprintf(stdout, "%s\n", msg.c_str());
        }
    }
#endif
}

std::ostringstream& DebugStream_::AppendToStream(LogLevel level, const char* file, int line) 
{
#ifdef DEBUG
    _lastLevel = level;

    stream <<  getLogLevelString(level) << "-DEBUG ";

    // timestamp
    time_t rawtime;
    time (&rawtime);
    struct tm *timeinfo = localtime (&rawtime);
    char buffer [80];
    strftime (buffer,80,"%m/%d %H:%M:%S",timeinfo);
    stream << buffer << " ";

    stream << BASENAME(file) << ":" << line  << "] ";
    stream << std::string(level > geoml::LogLevel_DEBUG ? level - geoml::LogLevel_DEBUG : 0, '\t');
#endif
    return stream;

}

#endif

} // end namespace geoml
