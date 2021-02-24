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
		printErrorToConsole("cannot assign x, bad_alloc");
		return InitGridErrors::UNEXPECTED_ERROR;
	}

	for (size_t i = 0; i < n - 1; ++i)
	{	//проверка на монотонность
		if (x[i + 1] <= x[i])
			return InitGridErrors::NON_MONOTONIC_DATA;
		//рассчитываем шаги
		h[i] = x[i + 1] - x[i];
	}

	return InitGridErrors::NO_ERRORS;
}
