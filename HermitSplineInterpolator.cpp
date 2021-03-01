#include "HermitSplineInterpolator.h"
#include "Grid.h"
#include "ErrorsHandler.h"

struct HermitSplineInterpolatorP
{
	HermitSplineInterpolatorP() :
		der_a{ 0.0 }, der_b{ 0.0 }, isDerivatives{ false },
		initialized{ false }
	{}

	bool initializeGrid()
	{
		Grid::InitGridErrors initGridErrors = grid.initializeGrid();

		switch (initGridErrors)
		{
		case Grid::InitGridErrors::NON_MONOTONIC_DATA:
			errorsHandler.pushBackError("x is non monotonic");
			break;
		case Grid::InitGridErrors::EMPTY_X:
			errorsHandler.pushBackError("x is empty");
			break;
		case Grid::InitGridErrors::BAD_ALLOC:
			errorsHandler.pushBackError("bad allocation has occured");
			break;
		default:
			break;
		}

		if (initGridErrors != Grid::InitGridErrors::NO_ERRORS)
		{
			errorsHandler.pushBackError("cannot initialize grid");
			return false;
		}

		return true;
	}
	void initialize()
	{
		if (initialized)
			return;

		if (y.size() == 0)
		{
			errorsHandler.pushBackError("y is not set");
			errorsHandler.invoke(&errorsHandler);
			return;
		}

		if (grid.get_x().size() != y.size())
		{
			errorsHandler.pushBackError("x size and y size do not match");
			errorsHandler.invoke(&errorsHandler);
			return;
		}

		if (!initializeGrid())
		{
			errorsHandler.invoke(&errorsHandler);
			return;
		}

		if (!get_m())
		{
			errorsHandler.invoke(&errorsHandler);
			return;
		}

		initialized = true;
	}
	bool get_m()
	{
		size_t n{ grid.get_n() };
		//std::vector<double> w;// weights

		try
		{
			m.resize(n);
			//w.resize(n - 1);
		}
		catch (std::bad_alloc& ex)
		{
			(void)ex;
			errorsHandler.pushBackError("cannot initialize ermit spline data, bad_alloc");

			return false;
		}

		const std::vector<double>& h{ grid.get_h() };
		double lambda{ 0.0 }, mu{ 0.0 };

		return true;
	}

	std::vector<double> y;
	std::vector<double> m;
	Grid grid;
	double der_a, der_b;
	ErrorsHandler errorsHandler;
	bool isDerivatives;
	bool initialized;
};

HermitSplineInterpolator::HermitSplineInterpolator()
{
	imp = new HermitSplineInterpolatorP;
}

HermitSplineInterpolator::~HermitSplineInterpolator()
{
	delete imp;
}

void HermitSplineInterpolator::set_x(const std::vector<double>& x)
{
	if (Grid::InitGridErrors::BAD_ALLOC == imp->grid.set_x(x))
		imp->errorsHandler.pushBackError("cannot x data, bad_alloc");
}

void HermitSplineInterpolator::set_y(const std::vector<double>& y)
{
	try
	{
		this->imp->y = y;
	}
	catch (std::bad_alloc& ex)
	{
		(void)ex;
		imp->errorsHandler.pushBackError("cannot y data, bad_alloc");
	}
}

double HermitSplineInterpolator::interpolate(double _x)
{
	return 0.0;
}

void HermitSplineInterpolator::setBoundaryConditions(double der_a, double der_b)
{
	imp->der_a = der_a;
	imp->der_b = der_b;
}

void HermitSplineInterpolator::initialize()
{
}
