#include "HermiteSplineInterpolator.h"
#include "Grid.h"
#include "ErrorsHandler.h"
#include "TDMA.h"

struct HermiteSplineInterpolatorP
{
	HermiteSplineInterpolatorP() :
		der_a{ 0.0 }, der_b{ 0.0 }, isDerivatives{ false },
		initialized{ false }
	{}

	bool initializeGrid()
	{
		Grid::InitGridErrors initGridErrors = grid.initializeGrid();

		switch (initGridErrors)
		{
		case Grid::InitGridErrors::NOT_ASCENDING:
			errorsHandler.pushBackError("x is not ascending");
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

		initializeBC();

		if (!get_m())
		{
			errorsHandler.invoke(&errorsHandler);
			return;
		}

		initialized = true;
	}
	void initializeBC()// initialization of boundary conditions
	{
		if (!isDerivatives)
		{
			size_t n = grid.get_n();
			const std::vector<double> &h = grid.get_h();

			der_a = (y[1] - y[0]) / h[0];
			der_b = (y[n - 1] - y[n - 2]) / h[n - 2];
		}
	}
	bool get_m()
	{
		size_t n{ grid.get_n() };
		std::vector<double> a, b, c, d;

		try
		{
			m.resize(n);
			w = std::move(std::vector<double>(n - 1, 1.0));
			a.resize(n);
			b.resize(n);
			c.resize(n);
			d.resize(n);
		}
		catch (std::bad_alloc& ex)
		{
			(void)ex;
			errorsHandler.pushBackError("cannot initialize ermit spline data, bad_alloc");

			return false;
		}

		createCoefficients(a, b, c, d);
		TDM_algorithm.setBounds(der_a, der_b);
		TDM_algorithm.calc(a, b, c, d, m);

		if (TDM_algorithm.errors.size() > 0)
		{
			for (auto& error : TDM_algorithm.errors)
				errorsHandler.pushBackError(error);

			return false;
		}

		return true;
	}
	void createCoefficients(std::vector<double>& a,
		std::vector<double>& b,
		std::vector<double>& c,
		std::vector<double>& d)
	{
		const std::vector<double>& h = grid.get_h();
		size_t n = a.size();
		size_t nn = h.size();

		a[0] = 0.0;
		a[n - 1] = (w[nn - 2] * h[nn - 1]) / (w[nn - 2] * h[nn - 1] + w[nn - 1] * h[nn - 2]);
		c[n - 1] = 0.0;
		c[0] = 1.0;
		b[0] = b[n - 1] = 2.0;
		d[0] = 3.0 * c[0] * (y[1] - y[0]) / h[0];

		for (size_t i = 1; i < n - 1; ++i)
		{
			b[i] = 2.0;
			a[i] = (w[i - 1] * h[i]) / (w[i - 1] * h[i] + w[i] * h[i - 1]);
			c[i] = 1.0 - a[i];
			d[i] = 3.0 * (a[i] * (y[i] - y[i - 1]) / h[i - 1] + c[i] * (y[i + 1] - y[i]) / h[i]);
		}

		d[n - 1] = 3.0 * a[n - 1] * (y[n - 1] - y[n - 2]) / h[n - 2];
	}

	std::vector<double> y;
	std::vector<double> m;
	std::vector<double> w;// weights
	Grid grid;
	TDMA TDM_algorithm;
	double der_a, der_b;
	ErrorsHandler errorsHandler;
	bool isDerivatives;
	bool initialized;
};

HermiteSplineInterpolator::HermiteSplineInterpolator()
{
	imp = new HermiteSplineInterpolatorP;
}

HermiteSplineInterpolator::~HermiteSplineInterpolator()
{
	delete imp;
}

void HermiteSplineInterpolator::set_x(const std::vector<double>& x)
{
	if (Grid::InitGridErrors::BAD_ALLOC == imp->grid.set_x(x))
		imp->errorsHandler.pushBackError("cannot x data, bad_alloc");
}

void HermiteSplineInterpolator::set_y(const std::vector<double>& y)
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

double HermiteSplineInterpolator::interpolate(double _x)
{
	if (imp->initialized)
	{
		const std::vector<double>& x = imp->grid.get_x();
		const std::vector<double>& h = imp->grid.get_h();
		size_t n{ x.size() };
		double t{ 0.0 };

		if (_x<x[0] || _x>x[n - 1])
			return 0.0;

		for (size_t i = 0; i < n - 1; ++i)
		{
			if (_x > x[i + 1])
				continue;
			if (_x == x[0])
				return imp->y[0];
			if (_x == x[n - 1])
				return imp->y[n - 1];

			t = (_x - x[i]) / h[i];

			return pow((1.0 - t), 2) * (1 + 2.0 * t) * imp->y[i] + pow(t, 2) * (3.0 - 2.0 * t) * imp->y[i + 1] + t * pow((1.0 - t), 2) * h[i] * imp->m[i] - pow(t, 2) * (1.0 - t) * h[i] * imp->m[i + 1];
		}
	}

	return 0.0;
}

void HermiteSplineInterpolator::setBoundaryConditions(double der_a, double der_b)
{
	imp->der_a = der_a;
	imp->der_b = der_b;
	imp->isDerivatives = true;
}

void HermiteSplineInterpolator::initialize()
{
	imp->initialize();
}

bool HermiteSplineInterpolator::isInitialized()
{
	return imp->initialized;
}

void HermiteSplineInterpolator::setErrorsHandlerDelegate(std::function<void(void* object)> _delegate)
{
	imp->errorsHandler.setErrorsHandlerDelegate(_delegate);
}
