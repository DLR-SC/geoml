/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2014-02-07 Martin Siggel <Martin.Siggel@dlr.de>
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
