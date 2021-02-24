#ifndef CUBICSPLINEINTERPOLATOR_H
#define CUBICSPLINEINTERPOLATOR_H

#include <vector>
#include <fstream>
#include <string>
#include <functional>

#include "Grid.h"
#include "TDMA.h"
#include "InterpolationDataLoader.h"
#include "ErrorsHandler.h"

class CubicSplineInterpolator
{
public:
	void set_x(const std::vector<double>& x);
	void set_y(const std::vector<double>& x);
	void initialize();
	double interpolate(double x);
	bool isInitialized() const
	{
		return initialized;
	}

	ErrorsHandler errorsHandler;

private:
	Grid grid;
	TDMA TDMA_algorith;
	std::vector<double> y;
	std::vector<double> m;
	bool initialized = false;
	double _a, _b, _d;

	void createCoefficients(std::vector<double>& a,
		                    std::vector<double>& b,
		                    std::vector<double>& c,
		                    std::vector<double>& d);
};

#endif // !CUBICSPLINEINTERPOLATOR_H


