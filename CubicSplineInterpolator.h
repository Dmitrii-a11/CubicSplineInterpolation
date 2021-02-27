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
	CubicSplineInterpolator();

	void set_x(const std::vector<double>& x);
	void set_y(const std::vector<double>& x);
	void setDerivatives(double firstOrder_a, double firstOrder_b, double secondOrder_a, double secondOrder_b);
	void initialize();
	double interpolate(double x);
	bool isInitialized() const
	{
		return initialized;
	}

	ErrorsHandler errorsHandler;

private:
	Grid grid;
	TDMA TDM_algorithm;
	std::vector<double> y;
	std::vector<double> m;
	bool initialized;
	double _a, _b, _d;
	double firstOrder_a, firstOrder_b, secondOrder_a, secondOrder_b;
	bool isDerivatives;

	void createCoefficients(std::vector<double>& a,
		                    std::vector<double>& b,
		                    std::vector<double>& c,
		                    std::vector<double>& d);
	bool initializeGrid();
	bool get_m();
	bool getResultFromTDMA();//get m from TDMA
	bool getResult();//get m without TDMA
};

#endif // !CUBICSPLINEINTERPOLATOR_H


