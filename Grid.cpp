#include "Grid.h"

Grid::Grid::InitGridErrors Grid::Grid::initializeGrid()
{
	n = x.size();

	if (n == 0)
		return InitGridErrors::EMPTY_X;

	try
	{
		h.resize(n - 1);
	}
	catch (std::bad_alloc& ex)
	{
		(void)ex;
		return InitGridErrors::BAD_ALLOC;
	}

	for (size_t i = 0; i < n - 1; ++i)
	{
		if (x[i + 1] <= x[i])
			return InitGridErrors::NOT_ASCENDING;
		h[i] = x[i + 1] - x[i];
	}

	return InitGridErrors::NO_ERRORS;
}
