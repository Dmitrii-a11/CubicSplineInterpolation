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
	const double NULL_INTERP_VALUE = 0.0;
}

class HermiteSplineInterpolator : public Interpolator
{
public:
	HermiteSplineInterpolator();
	virtual ~HermiteSplineInterpolator();

	virtual void set_x(const std::vector<double>& x) override;
	virtual void set_y(const std::vector<double>& y) override;
	virtual void set_x(std::vector<double>&& x) noexcept override;
	virtual void set_y(std::vector<double>&& y) noexcept override;
	virtual inline double interpolate(double x) override;

	void setBoundaryConditions(double der_a, double der_b);
	void initialize();
	bool isInitialized();
	void reset();
	void setErrorsHandlerDelegate(std::function<void(void* object)> _delegate);
	void setWeights(const std::vector<double>& w);
	void setWeights(std::vector<double>&& w) noexcept;
	void setWeightsCalculating(bool value);
	bool isWeightsCalculating() const;
	void setWeightsParameters(double c, double beta,
		                      double minWeightValue = HermiteSplineDefaultValues::MIN_WEIGHT);
	double getMinWeightsValue() const;
	void setNullInterpolateValue(double value);
	double getNullInterpValue() const;

private:
	HermiteSplineInterpolatorP* imp;
};

#endif //HERMITSPLINEINTERPOLATOR_H

