#ifndef GRID_H
#define GRID_H

#include <vector>
#include <iostream>
#include <string>

class Grid
{
public:
	enum class InitGridErrors { EMPTY_X, NOT_ASCENDING, BAD_ALLOC, NO_ERRORS };

	InitGridErrors set_x(const std::vector<double>& _x)
	{
		try
		{
			x = _x;
		}
		catch (std::bad_alloc& ex)
		{
			(void)ex;
			return InitGridErrors::BAD_ALLOC;
		}

		return InitGridErrors::NO_ERRORS;
	}

	void set_x(std::vector<double>&& _x) noexcept
	{
		x = std::move(_x);
	}

	InitGridErrors initializeGrid();

	const std::vector<double>& get_x() const
	{
		return x;
	}

	double get_x(size_t index)
	{
		return x[index];
	}

	double get_h(size_t index)
	{
		return h[index];
	}

	const std::vector<double>& get_h() const
	{
		return h;
	}

	size_t get_n() const
	{
		return n;
	}

	double operator [](size_t index)
	{
		return x[index];
	}

private:
	size_t n = 0;
	std::vector<double> x;
	std::vector<double> h;

	void printErrorToConsole(std::string message) const
	{
		std::cout << message << std::endl;
	}
};

#endif

