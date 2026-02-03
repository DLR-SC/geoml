#ifndef AD_UTILITY_FUNCTION_H
#define AD_UTILITY_FUNCTION_H


template<typename T>
double getPrimal(const T& x);

template<>
inline double getPrimal<double>(const double& x)
{
    return x;
} 

#if defined(GEOML_ADOLC_FORWARD) or defined(GEOML_ADOLC_REVERSE)
#include <Standard_TypeDef.hxx>

template<>
inline double getPrimal<Standard_Real>(const Standard_Real& x)
{   
    return x.getValue();
}

template<>
inline double getPrimal<adtl::adouble>(const adtl::adouble& x)
{   
    return x.getValue();
}

#endif

#endif // AD_UTILITY_FUNCTION_H