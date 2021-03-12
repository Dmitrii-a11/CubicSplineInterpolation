#include "CubicSplineInterpolator.h"
#include "Grid.h"
#include "TDMA.h"
#include "ErrorsHandler.h"

struct CubicSplineInterpolatorP
{
    CubicSplineInterpolatorP() :
        initialized{ false }, _a{ 0.0 }, _b{ 0.0 }, _d{ 0.0 },
        firstOrder_a{ 0.0 }, firstOrder_b{ 0.0 }, secondOrder_a{ 0.0 },
        secondOrder_b{ 0.0 }, isDerivatives{ 0.0 }
    {}

    Grid grid;
    TDMA TDM_algorithm;
    std::vector<double> y;
    std::vector<double> m;
    bool initialized;
    double _a, _b, _d;
    double firstOrder_a, firstOrder_b, secondOrder_a, secondOrder_b;
    bool isDerivatives;
    ErrorsHandler errorsHandler;

    void createCoefficients(std::vector<double>& a,
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
    bool get_m()
    {
        bool answer{ false };

        if (isDerivatives)
            answer = getResult();
        else
            answer = getResultFromTDMA();

        return answer;
    }
    bool getResultFromTDMA()//get m from TDMA
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
    bool getResult()//get m without TDMA but derivatives
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
};

CubicSplineInterpolator::CubicSplineInterpolator() 
{
    imp = new CubicSplineInterpolatorP;
}

CubicSplineInterpolator::~CubicSplineInterpolator()
{
    delete imp;
}

void CubicSplineInterpolator::set_x(const std::vector<double>& x)
{
    if (Grid::InitGridErrors::BAD_ALLOC == imp->grid.set_x(x))
        imp->errorsHandler.pushBackError("cannot x data, bad_alloc");
}

void CubicSplineInterpolator::set_y(const std::vector<double>& y)
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

void CubicSplineInterpolator::setDerivatives(double firstOrder_a, double firstOrder_b, double secondOrder_a, double secondOrder_b)
{
    this->imp->firstOrder_a = firstOrder_a;
    this->imp->firstOrder_b = firstOrder_b;
    this->imp->secondOrder_a = secondOrder_a;
    this->imp->secondOrder_b = secondOrder_b;
    imp->isDerivatives = true;
}

void CubicSplineInterpolator::initialize()
{
    if (imp->initialized)
        return;

    if (imp->y.size() == 0)
    {
        imp->errorsHandler.pushBackError("y is not set");
        imp->errorsHandler.invoke(&imp->errorsHandler);
        return;
    }

    if (imp->grid.get_x().size() != imp->y.size())
    {
        imp->errorsHandler.pushBackError("x size and y size do not match");
        imp->errorsHandler.invoke(&imp->errorsHandler);
        return;
    }

    if (!imp->initializeGrid())
    {
        imp->errorsHandler.invoke(&imp->errorsHandler);
        return;
    }
    
    if (!imp->get_m())
    {
        imp->errorsHandler.invoke(&imp->errorsHandler);
        return;
    }

    imp->initialized = true;
}

double CubicSplineInterpolator::interpolate(double _x)
{
    if (imp->initialized)
    {
        const std::vector<double>& x = imp->grid.get_x();
        const std::vector<double>& h = imp->grid.get_h();
        size_t n = x.size();

        for (size_t i = 1; i < n; ++i)
        {
            if (_x > x[i])
                continue;
            if (_x == x[0])
                return imp->y[0];
            if (_x == x[n - 1])
                return imp->y[n - 1];

            imp->_a = imp->y[i];
            imp->_d = (imp->m[i] - imp->m[i - 1]) / h[i - 1];

            if (imp->isDerivatives && i == n-1)
                imp->_b = imp->firstOrder_b;
            else
                imp->_b = (imp->m[i] * h[i - 1]) / 2.0 - (imp->_d * h[i - 1] * h[i - 1]) / 6.0 + (imp->y[i] - imp->y[i - 1]) / h[i - 1];

            return imp->_a + imp->_b * (_x - x[i]) + (imp->m[i] / 2.0) * (_x - x[i]) * (_x - x[i]) + (imp->_d / 6.0) * (_x - x[i]) * (_x - x[i]) * (_x - x[i]);
        }
    }

    return 0.0;
}

bool CubicSplineInterpolator::isInitialized() const
{
    return imp->initialized;
}

void CubicSplineInterpolator::setErrorsHandlerDelegate(std::function<void(void* object)> _delegate)
{
    imp->errorsHandler.setErrorsHandlerDelegate(_delegate);
}

//void CubicSplineInterpolator::createCoefficients(std::vector<double>& a,
//                                                 std::vector<double>& b,
//                                                 std::vector<double>& c,
//                                                 std::vector<double>& d)
//{
//    const std::vector<double>& h = grid.get_h();
//    const std::vector<double>& x = grid.get_x();
//    size_t n = a.size();
//
//    a[0] = 0.0;
//    a[n - 1] = h[n - 2];
//    c[n - 1] = 0.0;
//    c[0] = h[0];
//    d[0] = 6.0 * ((y[1] - y[0]) / h[0]);
//    d[n - 1] = -6.0 * ((y[n - 1] - y[n - 2]) / h[n - 2]);
//
//    for (size_t i = 1; i < n - 1; ++i)
//    {
//        a[i] = h[i - 1];
//        c[i] = h[i];
//        b[i] = 2.0 * (a[i] + c[i]);
//        d[i] = 6.0 * (((y[i + 1] - y[i]) / h[i]) - ((y[i] - y[i - 1]) / h[i - 1]));
//    }
//
//    b[0] = 2.0 * c[0];
//    b[n - 1] = 2.0 * a[n - 1];
//}

//bool CubicSplineInterpolator::initializeGrid()
//{
//    Grid::InitGridErrors initGridErrors = grid.initializeGrid();
//
//    switch (initGridErrors)
//    {
//    case Grid::InitGridErrors::NON_MONOTONIC_DATA:
//        errorsHandler.pushBackError("x is non monotonic");
//        break;
//    case Grid::InitGridErrors::EMPTY_X:
//        errorsHandler.pushBackError("x is empty");
//        break;
//    case Grid::InitGridErrors::BAD_ALLOC:
//        errorsHandler.pushBackError("bad allocation has occured");
//        break;
//    default:
//        break;
//    }
//
//    if (initGridErrors != Grid::InitGridErrors::NO_ERRORS)
//    {
//        errorsHandler.pushBackError("cannot initialize grid");       
//        return false;
//    }
//
//    return true;
//}

/*bool CubicSplineInterpolator::getResultFromTDMA()
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
}*/
