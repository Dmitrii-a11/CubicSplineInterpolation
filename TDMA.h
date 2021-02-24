#ifndef TDMA_H
#define TDMA_H

#include "Grid.h"

#include <vector>
#include <string>

class TDMA
{
public:
	TDMA();

	void calc(const std::vector<double>& a,
			  const std::vector<double>& b,
		      const std::vector<double>& c,
		      const std::vector<double>& d,
			        std::vector<double>& result);

	void setBounds(double firstBound, double lastBound)
	{
		this->firstBound = firstBound;
		this->lastBound = lastBound;
	}

	std::vector<std::string> errors;

private:
	std::vector<double>  alpha, beta;
	double firstBound = 0.0, lastBound = 0.0;

	void calc_alpha_beta(const std::vector<double>& a,
		                 const std::vector<double>& b,
		                 const std::vector<double>& c,
		                 const std::vector<double>& d);
};

#define TDMA_H
#endif

