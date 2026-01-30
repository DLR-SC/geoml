#ifndef AD_UTILITY_FUNCTION_H
#define AD_UTILITY_FUNCTION_H


template<typename T>
double getPrimal(const T& x);

template<>
double getPrimal(const double& x)
{
    return x;
}

#if defined(GEOML_ADOLC_FORWARD) or defined(GEOML_ADOLC_REVERSE)
#include <Standard_TypeDef.hxx>

template<>
double getPrimal(const Standard_Real& x)
{   
    return x.getValue();
}
#endif

#endif // AD_UTILITY_FUNCTION_H