#include "HermiteSplineInterpolator.h"
#include "Grid.h"
#include "ErrorsHandler.h"
#include "TDMA.h"

#include <algorithm>

struct HermiteSplineInterpolatorP
{
	HermiteSplineInterpolatorP() :
		der_a{ 0.0 }, der_b{ 0.0 }, isDerivatives{ false },
		initialized{ false }, isWeights{ false }, weightsCalculating{ false },
		cParameter{ 1.0 }, betaParameter{ 1.0 },
		minWeightValue{ HermiteSplineDefaultValues::MIN_WEIGHT },
		nullInterpValue{ HermiteSplineDefaultValues::NULL_INTERP_VALUE }
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
	bool checkWeightsParameters()
	{
		size_t lastErrorsCount{ errorsHandler.errorsCount() };

		if (minWeightValue <= 0.0)
		{
			errorsHandler.pushBackError(
				"fail at setting minimum weight value: "
				"the value of minimum weight must be positive");
		}

		if (cParameter < HermiteSplineDefaultValues::MIN_C_PARAMETER)
		{
			errorsHandler.pushBackError(
				"fail at setting 'c' parameter for weight calculating: "
				"'c' must be gerater or equal " +
				std::to_string(HermiteSplineDefaultValues::MIN_C_PARAMETER));
		}

		if (betaParameter < HermiteSplineDefaultValues::MIN_BETA_PARAMETER)
		{
			errorsHandler.pushBackError(
				"fail at setting 'beta' parameter for weight calculating: "
				"'beta' must be greater or equal " + 
				std::to_string(HermiteSplineDefaultValues::MIN_BETA_PARAMETER));
		}

		return lastErrorsCount == errorsHandler.errorsCount();
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

		if (!initializeGrid())
		{
			errorsHandler.invoke(&errorsHandler);
			return;
		}

		if (grid.get_x().size() != y.size())
		{
			errorsHandler.pushBackError("x size and y size do not match");
			errorsHandler.invoke(&errorsHandler);
			return;
		}

		initializeBC();

		if (!checkWeightsParameters())
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
	void get_w()
	{
		size_t beginIndexAsc{ 0 }, beginIndexDes{ 0 }, endIndexAsc{ 0 }, endIndexDes{ 0 };
		bool isAscending{ false }, isDescending{ false }, beginFound{ false }, endFound{ false };
		size_t n{ grid.get_n() - 1 };
		size_t gridSize{ n + 1 };

		for (size_t i = 0; i < n; ++i)
		{
			if (y[i + 1] > y[i])
			{
				if (!isAscending)
				{
					beginIndexAsc = i;
					beginFound = true;
					isAscending = true;
				}

				endIndexAsc = i + 1;
			}
			else
			{
				isAscending = false;

				if (beginFound)
					endFound = true;
			}

			if (beginFound && i == n - 1)
				endFound = true;

			if (beginFound && endFound)
			{
				if (endIndexAsc - beginIndexAsc + 1 >= 3.0)// for calculating the weights it is needed at least 3 points
					get_w_from_diapason(beginIndexAsc, endIndexAsc);

				isAscending = false;
			}
		}

		beginFound = endFound = false;

		for (size_t i = 0; i < n; ++i)
		{
			if (y[i + 1] < y[i])
			{
				if (!isDescending)
				{
					beginIndexDes = i;
					beginFound = true;
					isDescending = true;
				}

				endIndexDes = i + 1;
			}
			else
			{
				isDescending = false;

				if (beginFound)
					endFound = true;
			}

			if (beginFound && i == n - 1)
				endFound = true;

			if (beginFound && endFound)
			{
				if (endIndexDes - beginIndexDes + 1 >= 3.0)// for calculating the weights it is needed at least 3 points
					get_w_from_diapason(beginIndexDes, endIndexDes);

				isDescending = false;
			}
		}
	}
	void get_w_from_diapason(size_t beginIndex, size_t endIndex)
	{
		const auto& h{ grid.get_h() };
		size_t n{ endIndex - 1 };

		w[beginIndex] = pow((1.0 + cParameter * pow((y[beginIndex + 1] - y[beginIndex]) / h[beginIndex], 2.0)), -betaParameter);

		for (size_t i = ++beginIndex; i <= n; ++i)
		{
			double left{ 0.0 }, right{ 0.0 };// left and right sides of inequality
			double df_left{ 0.0 }, df_right{ 0.0 };
			bool first_success{ false }, second_success{ false };// conditions of the first and the second inequalities
			
			df_left = ((y[i] - y[i - 1]) / h[i - 1]) / ((y[i + 1] - y[i]) / h[i]);
			df_right = ((y[i + 1] - y[i]) / h[i]) / ((y[i] - y[i - 1]) / h[i - 1]);

			left = h[i] / h[i - 1];
			right = df_right - 2.0;

			if (left >= right)
				first_success = true;

			left = 1.0 / left;
			right = df_left - 2.0;

			if (left >= right)
				second_success = true;

			if (first_success && second_success)
			{
				w[i] = w[i - 1];
				continue;
			}

			if (!first_success)
				w[i] = w[i - 1] * h[i] / (h[i - 1] * (df_right - 2.0));
			if (!second_success)
				w[i] = (w[i - 1] * h[i] / h[i - 1]) * (df_left - 2.0);

			if (w[i] < minWeightValue)
				w[i] = minWeightValue;
		}
	}
	bool get_m()
	{
		size_t n{ grid.get_n() };
		std::vector<double> a, b, c, d;

		try
		{
			m.resize(n);
			if (!isWeights)
			{
				w = std::move(std::vector<double>(n - 1,
					HermiteSplineDefaultValues::DEFAULT_WEIGHT)
						     );
				isWeights = true;
			}
			else
			{
				if (w.size() != n)
				{
					errorsHandler.pushBackError("wights count does not equal to grid size");
					return false;
				}
			}
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

		if (weightsCalculating)
			get_w();

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
		const std::vector<double>& h{ grid.get_h() };
		size_t n{ a.size() };
		size_t nn{ h.size() };

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
	bool isWeights, weightsCalculating;
	double cParameter, betaParameter, minWeightValue;
	double nullInterpValue;
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
		imp->errorsHandler.pushBackError("cannot set x data, bad_alloc");
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
		imp->errorsHandler.pushBackError("cannot set y data, bad_alloc");
	}
}

inline double HermiteSplineInterpolator::interpolate(double _x)
{
	if (imp->initialized)
	{
		const std::vector<double>& x{ imp->grid.get_x() };
		const std::vector<double>& h{ imp->grid.get_h() };
		size_t n{ x.size() };
		double t{ 0.0 };
		auto upperBountIter{ std::upper_bound(x.begin(), x.end(), _x) };
		auto index{ std::distance(x.begin(), upperBountIter) };

		if (_x < x[0] || _x > x[n - 1])// if _x is not in [a, b] => exit
			return 0.0;

		if (index == n)
			return imp->y[n - 1];

		t = (_x - x[index - 1]) / h[index - 1];

		return pow((1.0 - t), 2.0) * (1.0 + 2.0 * t) * imp->y[index - 1] + pow(t, 2.0) * (3.0 - 2.0 * t) * imp->y[index] + t * pow((1.0 - t), 2.0) * h[index - 1] * imp->m[index - 1] - pow(t, 2.0) * (1.0 - t) * h[index - 1] * imp->m[index];
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

void HermiteSplineInterpolator::reset()
{
	imp->initialized = false;
	imp->isDerivatives = false;
	imp->isWeights = false;
	imp->weightsCalculating = false;
}

void HermiteSplineInterpolator::setErrorsHandlerDelegate(std::function<void(void* object)> _delegate)
{
	imp->errorsHandler.setErrorsHandlerDelegate(_delegate);
}

void HermiteSplineInterpolator::setWeights(const std::vector<double>& w)
{
	try
	{
		imp->w = w;
		imp->isWeights = true;
	}
	catch (std::bad_alloc& ex)
	{
		(void)ex;
		imp->errorsHandler.pushBackError("cannot set weights, bad_alloc");
	}
}

void HermiteSplineInterpolator::setWeightsCalculating(bool value)
{
	imp->weightsCalculating = value;
}

bool HermiteSplineInterpolator::isWeightsCalculating() const
{
	return imp->weightsCalculating;
}

void HermiteSplineInterpolator::setWeightsParameters(double c, double beta, double minWeightValue)
{
	imp->cParameter = c;
	imp->betaParameter = beta;
	imp->minWeightValue = minWeightValue;
}

double HermiteSplineInterpolator::getMinWeightsValue() const
{
	return imp->minWeightValue;
}

void HermiteSplineInterpolator::setNullInterpolateValue(double value)
{
	imp->nullInterpValue = value;
}

double HermiteSplineInterpolator::getNullInterpValue() const
{
	return imp->nullInterpValue;
}
