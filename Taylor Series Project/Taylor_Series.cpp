#include <iostream>
#include <iomanip>
#include "rogstadsModdedFunctions.h"

using namespace std;


int main()
{
	int counter = 0;

	while (counter < 2) {

		const double th = 1e-4;
		cout << "Enter an argument for tan(x): ";
		double n;
		cin >> n;

		double ans1 = rogstadsSinExp(n, th);
		double ans2 = rogstadsCosExp(n, th);
		double ans3 = rogstadsTanFunction(n, th);

		//cout << "sin(" << setprecision(1) << n << ") = " << setprecision(5) << fixed << ans1 << endl;
		//cout << "cos(" << setprecision(1) << n << ") = " << setprecision(5) << fixed << ans2 << endl;

		cout << "tan(" << setprecision(1) << n << ") = " << setprecision(5) << fixed << ans3
			<< endl;

		counter++;

	}


	return 0;
}

