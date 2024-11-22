#include <iostream>
#include <string>
#include <limits>
using namespace std;

class Car
{
private:
	string make;
	string model;
	int* year_last_service;

public:
	Car(string make = "NA", string model = "NA") {
		this->make = make;
		this->model = model;
		year_last_service = new int;
	}



	Car& operator=(const Car& right_obj) {
		cout << endl << "Assignment operator is being called now...";

		if (this != &right_obj) {     //avoiding self-reference, like x = x;
			delete year_last_service; //deallocation for safety
			make = right_obj.make;
			model = right_obj.model;
			year_last_service = new int; //new allocation
			*year_last_service = *(right_obj.year_last_service); //deep copy from right_obj to this object
		}
		return *this;
	}

	//-------------------------------------------METHODS TO BE IMPLEMENTED--------------------------------------

	~Car() { //DESTRUCTOR TO BE IMPLEMENTED

		delete year_last_service;	//Frees allocated memory
	}

	Car(Car& t) { //COPY CONSTRUCTOR TO BE IMPLEMENTED

		make = t.make;
		model = t.model;

		year_last_service = new int;

		*year_last_service = *(t.year_last_service);
	}


	bool operator==(const Car& t) { //Assignment operator to be implemented
		if (make == t.make && model == t.model) {
			return true;
		}
		else { return false; }
	}


	void read(istream& in) { //read function used for the redefinition of operator>>

		cout << endl << "What is the make of the car?" << endl;
		getline(in, make);

		cout << endl << "What is the model?" << endl;
		getline(in, model);

	}

	//---------------------------------------------------------------------------------------------------------


	void setMake(string a_make) {
		make = a_make;
	}

	void setModel(string a_model) {
		model = a_model;
	}

	void setService() {
		do {
			cout << endl << "Please privde year of last service for the car: ";
			cin >> *year_last_service;
		} while (*year_last_service < 1940 || *year_last_service > 2024);
	}


	void display(ostream& out) const {
		cout << endl << "Car make: " << make << " | Car model: " << model << " | Last serviced in: " << *year_last_service;
	}


};

std::ostream& operator<<(std::ostream& out, const Car& a) {
	a.display(out);
	return out;
}

std::istream& operator>>(istream& in, Car& a)
{
	a.read(in);
	return in;
}



//---------------------AREA MAIN FOR TESTING--------------------------------------


int main()
{
	Car car_1, car_2;

	cout << endl << car_1; //default parameters used for car_1

	cin >> car_2; //car_2 provided from user

	cout << endl << car_2;

	Car car_3;
	car_3 = car_2;  //car_3 copied from car_2

	cout << endl << car_3;

	if (car_3 == car_2) cout << endl << "Cars are same";


	return 0;
}