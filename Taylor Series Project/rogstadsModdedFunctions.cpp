#include "rogstadsModdedFunctions.h"
double rogstadsCosExp(double x, double threshold)
{
	double top = 0.0;
	double sum = 0.0;
	int n = 0;
	while (true) {
		double term = pow(x, 2 * n) / rogstadsFact(2 * n);
		top = pow(-1, n);
		sum += term * top;
		if (term < threshold) { break; }

		n++;
	}

	return sum;
}
double rogstadsSinExp(double x, double threshold)
{
	double top = 0.0;
	double sum = 0.0;
	int n = 0;
	while (true) {
		double term = pow(x, (2 * n) + 1) / rogstadsFact((2 * n) + 1);
		top = pow(-1, n);
		sum += term * top;
		if (term < threshold) { break; }

		n++;
	}

	return sum;
}
long long rogstadsFact(int n)
{
	long long result = 1;
	while (n > 1) {
		result *= n--;
	}
	return result;
}
double rogstadsTanFunction(double m, double n)
{
	double total;

	total = rogstadsSinExp(m, n) / rogstadsCosExp(m, n);

	return total;

}