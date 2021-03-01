#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <vector>

#include "Grid.h"

class Iinterpolator
{
public:
	virtual ~Iinterpolator()
	{}

	virtual void set_x(const std::vector<double>& x) = 0;
	virtual void set_y(const std::vector<double>& y) = 0;
	virtual double interpolate(double) = 0;
};

#endif // !INTERPOLATOR_H

