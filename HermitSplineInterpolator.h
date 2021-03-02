#ifndef HERMITSPLINEINTERPOLATOR_H
#define HERMITSPLINEINTERPOLATOR_H

#include "Interpolator.h"

struct HermitSplineInterpolatorP;

class HermitSplineInterpolator : public Iinterpolator
{
public:
	HermitSplineInterpolator();
	virtual ~HermitSplineInterpolator();

	virtual void set_x(const std::vector<double>& x) override;
	virtual void set_y(const std::vector<double>& y) override;
	virtual double interpolate(double x) override;

	void setBoundaryConditions(double der_a, double der_b);
	void initialize();
	bool isInitialized();

private:
	HermitSplineInterpolatorP* imp;
};

#endif //HERMITSPLINEINTERPOLATOR_H

