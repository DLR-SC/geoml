/*
* Copyright (c) 2018 RISC Software GmbH
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

#include "Debugging.h"

#include <filesystem>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_Curve.hxx>

#include <sstream>
#include <cctype>

#include "logging/Logging.h"
#include "common/CommonFunctions.h"

namespace
{
void createDirs(const std::string& path)
{
    std::filesystem::path f(path);
    if (f.has_parent_path()) {
        std::filesystem::create_directories(f.parent_path());
    }
}
} // namespace

namespace geoml
{

GEOML_EXPORT std::string TracePoint::m_debugDataDir = "CrashInfo";

void dumpShape(const TopoDS_Shape& shape, const std::string& outputDir, const std::string& filename, int counter)
{
    std::stringstream ss;
    ss << outputDir << "/";
    if (counter >= 0) {
        ss << counter << '_';
    }
    ss << filename;
    ss << ".brep";
    const std::string& file = ss.str();

    try {
        createDirs(file);
        if (BRepTools::Write(shape, file.c_str()) == Standard_False) {
            LOG(WARNING) << "Failed to dump debug shape " << file;
        }
    }
    catch (const std::exception& e) {
        LOG(WARNING) << "Failed to dump debug shape " << file << ": " << e.what();
    }
}

TracePoint::TracePoint(const std::string& outputDir)
    : m_outputDir(outputDir)
    , m_counter(0)
{
    size_t pos = 0;
    for (std::string::iterator it = m_outputDir.begin(); it != m_outputDir.end(); ++it, pos++) {
        char& c = *it;
        if (!std::isalnum(c) && c != '/'  && c != '\\') {
#if defined _WIN32 || defined __WIN32__
            if (c != ':' || pos != 1) {
                 // Don't overwrite the drive letter on windows
                 c = '_';
            }
#else
            c = '_';
#endif
        }
    }
}

int TracePoint::hitCount() const
{
    return m_counter;
}

void TracePoint::operator++(int)
{
    m_counter++;
}

void TracePoint::dumpShape(const TopoDS_Shape& shape, const std::string& filename)
{
    geoml::dumpShape(shape, m_outputDir, filename, m_counter);
}

DebugScope::DebugScope(TracePoint& tp)
    : m_tp(tp)
{
}

DebugScope::~DebugScope()
{
    // dump shapes in case we are stack unwinding
    if (std::uncaught_exceptions()) {
        for (std::map<std::string, TopoDS_Shape>::iterator it = m_shapes.begin(); it != m_shapes.end(); ++it) {
            m_tp.dumpShape(it->second, it->first);
        }
    }
}

void DebugScope::addShape(const TopoDS_Shape& shape, const std::string& filename)
{
    m_shapes[filename] = shape;
}

void DebugScope::addShape(const Handle(Geom_Curve) &curve, const std::string &filename)
{
    addShape(BRepBuilderAPI_MakeEdge(curve).Edge(), filename);
}

void DebugScope::dumpShape(const TopoDS_Shape& shape, const std::string& filename)
{
    m_tp.dumpShape(shape, filename);
}

} // namespace geoml
