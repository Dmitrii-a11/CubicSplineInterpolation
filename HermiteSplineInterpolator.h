#ifndef HERMITSPLINEINTERPOLATOR_H
#define HERMITSPLINEINTERPOLATOR_H

#include "Interpolator.h"
#include <functional>

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

private:
	HermiteSplineInterpolatorP* imp;
};

#endif //HERMITSPLINEINTERPOLATOR_H

