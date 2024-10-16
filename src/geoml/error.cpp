/* 
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2013 German Aerospace Center (DLR)
*
* Created: 2010-08-13 Markus Litz <Markus.Litz@dlr.de>
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
