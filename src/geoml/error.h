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
