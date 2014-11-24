#include <NIHeaders.h>

#include <iostream>
#include <fstream>
#include <iomanip>

template <typename Scalar>
Scalar desiredRelativeError()
{
  return Eigen::NumTraits<Scalar>::epsilon() * 50.;
}

template <typename Scalar>
typename Eigen::Integrator<Scalar>::QuadratureRule quadratureRules(const size_t i)
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


/**
 * This integrand has a peak of height 4^alphaPeak at x = pi/4.
 */
template<typename Scalar>
class IntegrandPeakFunctor
{
public:
  Scalar operator()(const Scalar param) const
  {
    return pow(4., -m_alpha) / (pow(param - M_PI / 4., 2.) + pow(16., -m_alpha));
  }

  /**
   * @param alpha A parameter for varying the peak.
   */
  void setAlpha(const Scalar alpha) {m_alpha = alpha;}

  static Scalar integralPeak(const Scalar alpha)
  {
    Scalar factor = pow(4., alpha - 1.);
    return atan((4. - M_PI) * factor) + atan(M_PI * factor);
  }

private:
  Scalar m_alpha;
};

int test_peak(void)
{
    ofstream fout;
    fout.open("Peak_integration_test_output.txt");

    std::cout<<"Testing Int [0->1] 4^-alpha/(x-pi/4)^2 + 16^-alpha = atan( (4-pi)4^(alpha-1) )+atan(pi-4^(alpha-1))"<<std::endl;

    //typedef float Scalar;
    //typedef double Scalar;
    //typedef long double Scalar;
    typedef mpfr::mpreal Scalar;
    Scalar::set_default_prec(6);

    typedef Eigen::Integrator<Scalar> IntegratorType;
    typedef IntegrandPeakFunctor<Scalar> IntegrandPeakFunctorType;

    //compute the nodes and weights on the fly
    QuadratureKronrod<Scalar>::computeNodesAndWeights();

    IntegratorType eigenIntegrator(500);
    IntegrandPeakFunctorType integrandPeakFunctor;

    bool success = true;
    int counter = 0;
    const size_t numKeys = 12;

    for (size_t i = 0; i < numKeys; ++i)
    {
        Eigen::Integrator<Scalar>::QuadratureRule quadratureRule = quadratureRules<Scalar>(i);

        for (Scalar alpha = 0.; alpha < 18.; ++alpha)
        {
            success = true;
            integrandPeakFunctor.setAlpha(alpha);

            Scalar actual = eigenIntegrator.quadratureAdaptive(integrandPeakFunctor, Scalar(0.),Scalar(1.), Scalar(0.), desiredRelativeError<Scalar>(), quadratureRule);

            Scalar expected = IntegrandPeakFunctorType::integralPeak(alpha);

            if(fabs((Scalar)(expected - actual)) > desiredRelativeError<Scalar>() * fabs(expected))
            {
                fout << "\nrule " << i << "\n fabs(expected - actual) =" << fabs(expected - actual)
                          << "\n desiredRelativeError<Scalar>() * Abs(expected)= "
                          << desiredRelativeError<Scalar>() * fabs(expected)<<std::endl;

                fout << "errorCode = " << eigenIntegrator.errorCode() << std::endl;
                success = false;
            }
            else
            {
                fout << "\nrule " << i << "\n Abs(expected - actual) =" << fabs(expected - actual)
                          << "\n desiredRelativeError<Scalar>() * Abs(expected)= "
                          << desiredRelativeError<Scalar>() * fabs(expected) << std::endl;
                          
                fout << "Success!\n";
                counter++;
            }
        }
        
        if(success && counter == 18)    
        {
          fout << "\n  Test Succeeded!\n" << std::endl;
          fout.close();
          break;
        }
        else
        {
          fout <<"\n  Test Failed.\n" << std::endl;
        }
    }

    fout.close();

    if (success)
    {
      std::cout << std::endl << "  Test Succeeded!\n" << std::endl;
      return EXIT_SUCCESS;
    }
    else
    {
      std::cout << std::endl << "  Test Failed.\n" << std::endl;
      return EXIT_FAILURE;
    }
}


int main(void)
{
    int ret = EXIT_SUCCESS;
    ret += test_peak();
    return EXIT_SUCCESS;
}
