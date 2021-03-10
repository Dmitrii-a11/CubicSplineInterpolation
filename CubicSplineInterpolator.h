#ifndef CUBICSPLINEINTERPOLATOR_H
#define CUBICSPLINEINTERPOLATOR_H

//#include <vector>
//#include <fstream>
//#include <string>
#include <functional>
//
//#include "Grid.h"
//#include "TDMA.h"
//#include "InterpolationDataLoader.h"
//#include "ErrorsHandler.h"
#include "Interpolator.h"

struct CubicSplineInterpolatorP;

class CubicSplineInterpolator : public Iinterpolator
{
public:
	CubicSplineInterpolator();
	virtual ~CubicSplineInterpolator();

	virtual void set_x(const std::vector<double>& x) override;
	virtual void set_y(const std::vector<double>& x) override;
	void setDerivatives(double firstOrder_a, double firstOrder_b, double secondOrder_a, double secondOrder_b);
	void initialize();
	virtual double interpolate(double x) override;
	bool isInitialized() const;
	void setErrorsHandlerDelegate(std::function<void(void* object)> _delegate);

private:
	CubicSplineInterpolatorP* imp;
};

#endif // !CUBICSPLINEINTERPOLATOR_H


