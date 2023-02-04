#ifndef CUSTOMER_H
#define CUSTOMER_H

using namespace std;

class Customer{
	public: 
		Customer(); // default constructor
		double Demand;
		double X;
		double Y;
		
	private:
};

Customer::Customer(){
	Demand = 0.0;
	X = 0.0;
	Y = 0.0;
}
    
#endif