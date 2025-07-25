/*
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2014-02-07 Martin Siggel <Martin.Siggel@dlr.de>
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

#ifndef GEOML_INTERNAL_H
#define GEOML_INTERNAL_H

#if defined(WIN32)
  // define GEOML_INTERNAL_EXPORTS, if you want to expose the internal
  // api to the dll interface (just for testing purposes!)
  #if defined (GEOML_INTERNAL_EXPORTS)
    #define GEOML_EXPORT __declspec (dllexport)
  #elif defined (GEOML_INTERNAL_IMPORTS)
    #define GEOML_EXPORT __declspec (dllimport)
  #else
    #define GEOML_EXPORT
  #endif
#else
    #define GEOML_EXPORT
#endif

#define VERSION_HEX_CODE(MAJOR, MINOR, PATCH) ((MAJOR) << 16 | (MINOR) << 8 | (PATCH))

// defines the DEPRECATED macro

// from: http://stackoverflow.com/questions/295120/c-mark-as-deprecated/21265197#21265197
#if defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#elif defined(__GNUC__)
#define DEPRECATED __attribute__((deprecated))
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif

#endif // GEOML_INTERNAL_H
