#ifndef HERMITSPLINEINTERPOLATOR_H
#define HERMITSPLINEINTERPOLATOR_H

#include <functional>

#include "Interpolator.h"

struct HermiteSplineInterpolatorP;

namespace HermiteSplineDefaultValues
{
	const double MIN_WEIGHT = 1E-6;
	const double DEFAULT_WEIGHT = 1.0;
	const double C_PARAMETER = 1.0;
	const double BETA_PARAMETER = 1.0;
	const double MIN_C_PARAMETER = 1.0;
	const double MIN_BETA_PARAMETER = 0.0;
}

class HermiteSplineInterpolator : public Iinterpolator
{
public:

	HermiteSplineInterpolator();
	virtual ~HermiteSplineInterpolator();

	virtual void set_x(const std::vector<double>& x) override;
	virtual void set_y(const std::vector<double>& y) override;
	virtual double interpolate(double x) override;

	void setBoundaryConditions(double der_a, double der_b);
	void initialize();
	bool isInitialized();
	void reset();
	void setErrorsHandlerDelegate(std::function<void(void* object)> _delegate);
	void setWeights(const std::vector<double>& w);
	void setWeights(std::vector<double>&& w);
	void setWeightsCalculating(bool value);
	void setWeightsParameters(double c, double beta,
		                      double minWeightValue = HermiteSplineDefaultValues::MIN_WEIGHT);

private:
	HermiteSplineInterpolatorP* imp;
};

#endif //HERMITSPLINEINTERPOLATOR_H

