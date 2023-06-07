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

#include "geoml/error.h"
#include "logging/Logging.h"

namespace geoml 
{

// Constructor
Error::Error(std::string error, geoml::ReturnCode errorCode) noexcept
    : err(error)
    , code(errorCode)
{
    DLOG(ERROR) << "Error: ErrorCode=" << code << " : " << error;
}

// Destructor
Error::~Error() noexcept
{
}

const char* Error::what() const noexcept
{
   return err.c_str();
}

// Returns the error code
geoml::ReturnCode Error::getCode() const noexcept
{ 
    return code; 
}

} // end namespace geoml
