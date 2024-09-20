/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2018 RISC Software GmbH
*/

#pragma once

#include "geoml_internal.h"
#include <atomic>
#include <string>
#include <map>
#include <Geom_Curve.hxx>

class TopoDS_Shape;
class Geom_Curve;

namespace geoml
{
GEOML_EXPORT void dumpShape(const TopoDS_Shape& shape, const std::string& outputDir, const std::string& filename, int counter = -1);

// represents a point in the execution of a program at which shapes can be dumped
class TracePoint
{
public:
    GEOML_EXPORT TracePoint(const std::string& outputDir);

    GEOML_EXPORT void operator++(int);
    GEOML_EXPORT int hitCount() const;
    GEOML_EXPORT void dumpShape(const TopoDS_Shape& shape, const std::string& filename);

    GEOML_EXPORT static void setDebugDataDir(const std::string& dir)
    {
        m_debugDataDir = dir;
    }
    
    GEOML_EXPORT static std::string debugDataDir()
    {
        return m_debugDataDir;
    }

private:
    std::string m_outputDir;
    std::atomic<int> m_counter;

    GEOML_EXPORT static std::string m_debugDataDir;
};

// creates a trace point at the location of this macro with the specified variable name and output directory
// the trace point is static and incremented each time programm flow executes over the location of this macro
#define TRACE_POINT_OUTPUT_DIR(variableName, outputDir) static ::geoml::TracePoint variableName(outputDir); variableName++

// uses the function inside which the macro is expanded as output directory, __FUNCTION__ may not be supported by each compiler
#define TRACE_POINT(variableName) TRACE_POINT_OUTPUT_DIR(variableName, std::string(geoml::TracePoint::debugDataDir() + "/") + __FUNCTION__)

// allows dumping shapes in case an object of this class is destroyed by an exception
class DebugScope
{
public:
    GEOML_EXPORT DebugScope(TracePoint& tp);
    GEOML_EXPORT ~DebugScope();

    // stores shape and dumps it in case the scope is left by exception
    GEOML_EXPORT void addShape(const TopoDS_Shape& shape, const std::string& filename);

    GEOML_EXPORT void addShape(const Handle(Geom_Curve) & curve, const std::string& filename);

    // dumps the shape to the filesystem immediately
    GEOML_EXPORT void dumpShape(const TopoDS_Shape& shape, const std::string& filename);

private:
    TracePoint& m_tp;
    std::map<std::string, TopoDS_Shape> m_shapes;
};

// creates a debug scope object at the location of this macro with the specified variable name
// furthermore, a tracepoint is created, used by the debug scope to dump shapes in case of leaving the current scope by an exception
#define DEBUG_SCOPE(variableName) TRACE_POINT(variableName##TracePoint); ::geoml::DebugScope variableName(variableName##TracePoint)
}
