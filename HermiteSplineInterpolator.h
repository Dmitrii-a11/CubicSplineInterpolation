#ifndef HERMITSPLINEINTERPOLATOR_H
#define HERMITSPLINEINTERPOLATOR_H

#include <functional>

#include "Interpolator.h"

struct HermiteSplineInterpolatorP;

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
	void setErrorsHandlerDelegate(std::function<void(void* object)> _delegate);
	void setWeights(const std::vector<double>& w);
	void setWeights(std::vector<double>&& w);
	void setWeightsCalculating(bool value);

private:
	HermiteSplineInterpolatorP* imp;
};

#endif //HERMITSPLINEINTERPOLATOR_H

