#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <vector>

class Interpolator
{
public:
	virtual ~Interpolator()
	{}

	virtual void set_x(const std::vector<double>& x) = 0;
	virtual void set_y(const std::vector<double>& y) = 0;
	virtual void set_x(std::vector<double>&& x) {}
	virtual void set_y(std::vector<double>&& y) {}
	virtual double interpolate(double) = 0;
};

#endif // !INTERPOLATOR_H

