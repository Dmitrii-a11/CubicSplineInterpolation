#include "CubicSplineInterpolator.h"           

CubicSplineInterpolator::CubicSplineInterpolator() : 
    initialized{ false }, _a{ 0.0 }, _b{ 0.0 }, _d{ 0.0 },
    firstOrder_a{ 0.0 }, firstOrder_b{ 0.0 }, secondOrder_a{ 0.0 },
    secondOrder_b{ 0.0 }, isDerivatives{ 0.0 }
{}

void CubicSplineInterpolator::set_x(const std::vector<double>& x)
{
    if (Grid::InitGridErrors::BAD_ALLOC == grid.set_x(x))
        errorsHandler.pushBackError("cannot x data, bad_alloc");
}

void CubicSplineInterpolator::set_y(const std::vector<double>& y)
{
    try
    {
        this->y = y;
    }
    catch (std::bad_alloc& ex)
    {
        (void)ex;
        errorsHandler.pushBackError("cannot y data, bad_alloc");
    }
}

void CubicSplineInterpolator::setDerivatives(double firstOrder_a, double firstOrder_b, double secondOrder_a, double secondOrder_b)
{
    this->firstOrder_a = firstOrder_a;
    this->firstOrder_b = firstOrder_b;
    this->secondOrder_a = secondOrder_a;
    this->secondOrder_b = secondOrder_b;
    isDerivatives = true;
}

void CubicSplineInterpolator::initialize()
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

double CubicSplineInterpolator::interpolate(double _x)
{
    if (initialized)
    {
        const std::vector<double>& x = grid.get_x();
        const std::vector<double>& h = grid.get_h();
        size_t n = x.size();

        for (size_t i = 1; i < n; ++i)
        {
            if (_x > x[i])
                continue;
            if (_x == x[0])
                return y[0];
            if (_x == x[n - 1])
                return y[n - 1];

            _a = y[i];
            _d = (m[i] - m[i - 1]) / h[i - 1];

            if (isDerivatives && i == n-1)
                _b = firstOrder_b;
            else
                _b = (m[i] * h[i - 1]) / 2.0 - (_d * h[i - 1] * h[i - 1]) / 6.0 + (y[i] - y[i - 1]) / h[i - 1];

            return _a + _b * (_x - x[i]) + (m[i] / 2.0) * (_x - x[i]) * (_x - x[i]) + (_d / 6.0) * (_x - x[i]) * (_x - x[i]) * (_x - x[i]);
        }
    }

    return 0.0;
}

void CubicSplineInterpolator::createCoefficients(std::vector<double>& a,
                                                 std::vector<double>& b,
                                                 std::vector<double>& c,
                                                 std::vector<double>& d)
{
    const std::vector<double>& h = grid.get_h();
    const std::vector<double>& x = grid.get_x();
    size_t n = a.size();

    a[0] = 0.0;
    a[n - 1] = h[n - 2];
    c[n - 1] = 0.0;
    c[0] = h[0];
    d[0] = 6.0 * ((y[1] - y[0]) / h[0]);
    d[n - 1] = -6.0 * ((y[n - 1] - y[n - 2]) / h[n - 2]);

    for (size_t i = 1; i < n - 1; ++i)
    {
        a[i] = h[i - 1];
        c[i] = h[i];
        b[i] = 2.0 * (a[i] + c[i]);
        d[i] = 6.0 * (((y[i + 1] - y[i]) / h[i]) - ((y[i] - y[i - 1]) / h[i - 1]));
    }

    b[0] = 2.0 * c[0];
    b[n - 1] = 2.0 * a[n - 1];
}

bool CubicSplineInterpolator::initializeGrid()
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

bool CubicSplineInterpolator::getResultFromTDMA()
{
    size_t n{ grid.get_n() };
    std::vector<double> a, b, c, d;

    try
    {
        a.resize(n);
        b.resize(n);
        c.resize(n);
        d.resize(n);
        m.resize(n);
    }
    catch (std::bad_alloc& ex)
    {
        (void)ex;
        errorsHandler.pushBackError("cannot initialize cubic spline data, bad_alloc");
        
        return false;
    }

    createCoefficients(a, b, c, d);
    TDM_algorithm.calc(a, b, c, d, m);

    if (TDM_algorithm.errors.size() > 0)
    {
        for (auto& error : TDM_algorithm.errors)
            errorsHandler.pushBackError(error);

        return false;
    }
    return true;
}

bool CubicSplineInterpolator::get_m()
{
    bool answer{ false };

    if (isDerivatives)
        answer = getResult();
    else
        answer = getResultFromTDMA();

    return answer;
}

bool CubicSplineInterpolator::getResult()
{
    size_t n{ grid.get_n() };

    try
    {
        m.resize(n);
    }
    catch (std::bad_alloc& ex)
    {
        (void)ex;
        errorsHandler.pushBackError("cannot initialize cubic spline data, bad_alloc");

        return false;
    }

    const std::vector<double>& h{ grid.get_h() };

    m[0] = secondOrder_a;
    m[1] = (6.0 / h[0]) * ((y[1] - y[0]) / h[0] - firstOrder_a) - 2.0 * secondOrder_a;
    m[n - 1] = secondOrder_b;

    for (size_t i = 1; i < n - 1; ++i)
        m[i + 1] = (6.0 / h[i]) * ((y[i + 1] - y[i]) / h[i] - (y[i] - y[i - 1]) / h[i - 1]) - m[i - 1] - (2.0 / h[i]) * (h[i - 1] + h[i]) * m[i];

    return true;
}
