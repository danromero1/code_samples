#include <iostream>
using namespace std;
int main()
{
	int state = 1;
	int sodaCost = 75;
	int cokeInventory = 2;
	int squirtInventory = 2;
	int total = 0;
	int amount = 0;
	int change = 0;
	int option = 0;

	enum { ENTER$ = 1, WAITSEL, CHECKINV, RETURN };

	while (true) {

		switch (state) {

		case ENTER$:

			while (total <= sodaCost) {
				cout << "Insert 75 cents for a soda. Enter (-1) to return money." << endl;
				cin >> amount;


				if (amount == -1) { state = 4; total += amount + 1; break; }

				else { total += amount; }

				if (total >= sodaCost) {
					change = total - sodaCost; cout << change << " cents is your change." << endl;
						total = total - change; state = 2; change = 0; amount = 0; break;
				}

			}
			break;

		case WAITSEL:

			cout << "Select (1) for Coke and (2) for Squirt. Enter (-1) to return money." << endl;
			cin >> option;

			if (option == -1) { state = 4; }

			else if (option == 1 || option == 2) { state = 3; }

			break;

		case CHECKINV:

			if ((cokeInventory == false) && (squirtInventory == false)) {
				cout << "All sold out." << endl;
				state = 4;
				break;
			}

			else if (option == 1) {

				if (cokeInventory != false) {
					cout << "Coke dispensed." << endl;
					cokeInventory--;
					total = 0;
					state = 1;
					break;
				}

				else if (cokeInventory == false) {
					cout << "Sold out of your selection." << endl;
					state = 2;
					break;
				}
			}
			if (option == 2) {

				if (squirtInventory != false) {
					cout << "Squirt dispensed." << endl;
					squirtInventory--;
					total = 0;
					state = 1;
					break;
				}

				else if (squirtInventory == false) {
					cout << "Sold out of your selection. " << endl;
					state = 2;
					break;
				}
			}

		case RETURN:

			cout << total << " cents have dispensed." << endl; total = 0; state = 1;
			break;
		}
	}
	return 0;
}