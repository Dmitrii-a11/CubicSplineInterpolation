#include "TDMA.h"

TDMA::TDMA()
{}

void TDMA::calc(const std::vector<double>& a,
	            const std::vector<double>& b,
	            const std::vector<double>& c,
	            const std::vector<double>& d,
				      std::vector<double>& result)
{
	size_t n = a.size();

	try
	{
		alpha.resize(n);
		beta.resize(n);
	}
	catch (std::bad_alloc& ex)
	{
		(void)ex;
		errors.push_back("cannot initialize TDMA data, bad_alloc");
		return;
	}

	calc_alpha_beta(a, b, c, d);

	result[0] = firstBound;
	result[n - 1] = lastBound;

	for (size_t i = n - 1; i > 1; --i)
		result[i - 1] = alpha[i] * result[i] + beta[i];
}

void TDMA::calc_alpha_beta(const std::vector<double>& a,
	                       const std::vector<double>& b,
	                       const std::vector<double>& c,
	                       const std::vector<double>& d)
{
	alpha[0] = beta[0] = 0.0;

	size_t n = a.size();

	for (size_t i = 0; i < n - 1; ++i)
	{
		alpha[i+1] = -c[i] / (alpha[i] * a[i] + b[i]);
		beta[i+1] = (d[i] - a[i] * beta[i]) / (alpha[i ] * a[i] + b[i]);
	}
}
