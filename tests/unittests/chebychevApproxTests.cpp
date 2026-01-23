/* 
* Copyright (C) 2007-2014 German Aerospace Center (DLR/SC)
*
* Created: 2014-11-18 Martin Siggel <martin.siggel@dlr.de>
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


#include "test.h" 
#include "math/MathFunctions.h"
#include "geoml/error.h"

/******************************************************************************/

class ChebychevApproxTests : public ::testing::Test 
{
protected:
    static void SetUpTestCase() 
    {
    }

    static void TearDownTestCase() 
    {
    }

    void SetUp() override {}
    void TearDown() override {}
};

namespace
{
    typedef Standard_Real (*MathFunc)(Standard_Real t, void* obj);

    class MathFuncAdapter : public geoml::MathFunc1d
    {
    public:
        MathFuncAdapter(MathFunc func, void* obj)
            : _p(obj)
            , _func(func)
        {
        }

        Standard_Real value(Standard_Real t) override
        {
            return _func(t, _p);
        }

    private:
        void * _p;
        MathFunc _func;
    };

    // some functions to approximate
    Standard_Real parabola(Standard_Real x, void*)
    {
        return x*x;
    }
    
    Standard_Real line(Standard_Real x, void*)
    {
        return -5.*x;
    }
    
    Standard_Real shifted_parabola(Standard_Real x, void*)
    {
        return 2.*(x-4.)*(x-4.) - 1.;
    }
    
    Standard_Real cubic_with_parameter(Standard_Real x, void* p)
    {
        Standard_Real par = *(Standard_Real*)p;
        return par*x*x*x;
    }

}

TEST_F(ChebychevApproxTests, parabola)
{
    MathFuncAdapter adapt(parabola, NULL);
    math_Vector v = geoml::cheb_approx(adapt, 4, -1., 1.);
    ASSERT_NEAR(0.5, v(0).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(1).getValue(), 1e-12);
    ASSERT_NEAR(0.5, v(2).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(3).getValue(), 1e-12);
}

TEST_F(ChebychevApproxTests, line)
{
    MathFuncAdapter adapt(line, NULL);
    math_Vector v = geoml::cheb_approx(adapt, 4, -1., 1.);
    ASSERT_NEAR(0.0, v(0).getValue(), 1e-12);
    ASSERT_NEAR(-5., v(1).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(2).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(3).getValue(), 1e-12);
}

TEST_F(ChebychevApproxTests, line_invalid)
{
    MathFuncAdapter adapt(line, NULL);
    ASSERT_THROW(geoml::cheb_approx(adapt, 0, -1., 1.), geoml::Error);
   
}

TEST_F(ChebychevApproxTests, shifted_parabola)
{
    MathFuncAdapter adapt(shifted_parabola, NULL);
    math_Vector v = geoml::cheb_approx(adapt, 4, 3., 5.);
    ASSERT_NEAR(0.0, v(0).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(1).getValue(), 1e-12);
    ASSERT_NEAR(1.0, v(2).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(3).getValue(), 1e-12);
}

TEST_F(ChebychevApproxTests, cubic_with_parameter)
{
    Standard_Real parm = 7.;
    MathFuncAdapter adapt(cubic_with_parameter, &parm);
    math_Vector v = geoml::cheb_approx(adapt, 10, -1., 1.);
    ASSERT_NEAR(0.0, v(0).getValue(), 1e-12);
    ASSERT_NEAR(0.75*parm.getValue(), v(1).getValue(), 1e-12);
    ASSERT_NEAR(0.0    , v(2).getValue(), 1e-12);
    ASSERT_NEAR(parm.getValue()/4., v(3).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(4).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(5).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(6).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(7).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(8).getValue(), 1e-12);
    ASSERT_NEAR(0.0, v(9).getValue(), 1e-12);
}
