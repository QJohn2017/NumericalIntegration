/**
 * \file testIntegrator_Sine.cpp
 * This file is a unit test for Integrator.h and its' associated files.
 * The test is an (oscillatory) Sine function from zero to pi.
 */

#include <NumericalIntegration.h>

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace Eigen;

///////////////////////////// A simple example of sin(x) ///////////////////////
template<typename Scalar>
class IntegrandSineFunctor
{
public:
    Scalar operator()(const Scalar& param) const
    {
      using std::sin;
      return sin(param);
    }
};

/**
 * \param epsilon Relative machine precision.
 */
template <typename Scalar>
Scalar desiredRelativeError()
{
  return Eigen::NumTraits<Scalar>::epsilon() * 50.;
}

template <typename Scalar>
typename Eigen::Integrator<Scalar>::QuadratureRule quadratureRules(const size_t& i)
{
    static const typename Eigen::Integrator<Scalar>::QuadratureRule quadratureRules[12] =
    {
        Eigen::Integrator<Scalar>::GaussKronrod15,
        Eigen::Integrator<Scalar>::GaussKronrod21,
        Eigen::Integrator<Scalar>::GaussKronrod31,
        Eigen::Integrator<Scalar>::GaussKronrod41,
        Eigen::Integrator<Scalar>::GaussKronrod51,
        Eigen::Integrator<Scalar>::GaussKronrod61,
        Eigen::Integrator<Scalar>::GaussKronrod71,
        Eigen::Integrator<Scalar>::GaussKronrod81,
        Eigen::Integrator<Scalar>::GaussKronrod91,
        Eigen::Integrator<Scalar>::GaussKronrod101,
        Eigen::Integrator<Scalar>::GaussKronrod121,
        Eigen::Integrator<Scalar>::GaussKronrod201
    };

    return quadratureRules[i];
}

int test_sine(void)
{
    std::ofstream fout;
    fout.open("test/testOutput/Sine_integration_test_output.txt");

    std::cout<<"\nTesting Int [0->Pi] sin(x) = 2\n";
     
    //typedef float Scalar;
    typedef double Scalar;
    //typedef long double Scalar;
    //typedef mpfr::mpreal Scalar;  // \detail Performing this test using multiprecision requires changing from M-PI to NumTraits<Scalar>::PI();
    //Scalar::set_default_prec(500);

    typedef Eigen::Integrator<Scalar> IntegratorType;
    typedef IntegrandSineFunctor<Scalar> IntegrandSineFunctorType;

    IntegratorType eigenIntegrator(1000);
    IntegrandSineFunctorType integrandSineFunctor;

    bool success = true;
    const size_t numRules = 12;

    for (size_t i = 0; i < numRules; ++i)
    {
        success = true;

        Eigen::Integrator<Scalar>::QuadratureRule quadratureRule = quadratureRules<Scalar>(i);

        // \TODO The usage of NumTraits<Scalar>::Pi() is required for multiprecision
        //Scalar actual = eigenIntegrator.quadratureAdaptive(integrandSineFunctor, Scalar(0.), NumTraits<Scalar>::Pi(), Scalar(0.), desiredRelativeError<Scalar>(), quadratureRule);
        Scalar actual = eigenIntegrator.quadratureAdaptive(integrandSineFunctor, Scalar(0.), Scalar(M_PI), Scalar(0.), desiredRelativeError<Scalar>(), quadratureRule);
        Scalar expected = Scalar(2);

        using std::abs;
        if(abs((Scalar)(expected - actual)) > desiredRelativeError<Scalar>() * abs(expected) 
                || std::isnan(abs((Scalar)(expected - actual))))
        {
            fout << "\nrule " << i << "\n abs(expected - actual) = " << abs(expected - actual)
                 << "\n desiredRelativeError<Scalar>() * abs(expected) = "
                 << desiredRelativeError<Scalar>() * abs(expected) << std::endl;

          fout << "errorCode = " << eigenIntegrator.errorCode() << std::endl;
          success = false;
        }
        else
        {
            fout << "\nrule " << i << "\n abs(expected - actual) = " << abs(expected - actual)
                 << "\n desiredRelativeError<Scalar>() * abs(expected) = "
                 << desiredRelativeError<Scalar>() * abs(expected) << std::endl;

            fout << "errorCode = " << eigenIntegrator.errorCode() << std::endl;
            fout << "Success!\n ";
        }

        if(success)
        {
            fout << "\n\tTest Succeeded!\n" << std::endl;
            fout.close();
            break;
        }
        else
        {
            fout <<"\n\tTest Failed.\n" << std::endl;
        }
    }

    fout.close();

    if (success)
    {
        std::cout << std::endl << "\tTest Succeeded!\n" << std::endl;
        return EXIT_SUCCESS;
    }
    else
    {
        std::cout << std::endl << "\tTest Failed.\n" << std::endl;
        return EXIT_FAILURE;
    }
}

int main(void)
{
    int ret = EXIT_SUCCESS;
    ret += test_sine();
    return EXIT_SUCCESS;
}
