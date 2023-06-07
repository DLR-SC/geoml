/* 
* Copyright (C) 2007-2013 German Aerospace Center (DLR/SC)
*
* Created: 2010-08-13 Markus Litz <Markus.Litz@dlr.de>
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
* @brief  Exception class used to throw geoml exceptions.
*/

#ifndef Error_H
#define Error_H

#include <geoml/exports.h>
#include <string>
#include <exception>

namespace geoml 
{

enum ReturnCode
{
    GENERIC_ERROR,
    NOT_FOUND,
    MATH_ERROR,
    NULL_POINTER,
    INDEX_ERROR,
    OPEN_FAILED
};

class Error : public std::exception
{

public:
    // Constructor
    GEOML_API_EXPORT Error(std::string error = "", geoml::ReturnCode errorCode = geoml::GENERIC_ERROR) noexcept;

    // Destructor
    GEOML_API_EXPORT ~Error() noexcept override;

    // Default copy constructor and assignment operator are correct since
    // memberwise copy is enough for this class.

    GEOML_API_EXPORT const char* what() const noexcept override;

    // Returns the error code
    GEOML_API_EXPORT virtual geoml::ReturnCode getCode() const noexcept;

private:
    std::string    err;
    geoml::ReturnCode code;
    
};

} // end namespace geoml

#endif // Error_H
