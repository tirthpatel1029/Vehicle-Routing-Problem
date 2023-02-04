#include <iostream>
#include <fstream>
#include <limits>	// To use infinite value
#include <cmath>
#include <iomanip>
#include <string>
#include <vector>
#include <time.h>
#include "customer.h"

using namespace std;

int maxRunTime = 600;
time_t startTime;		// Variable to hold the starting time of backtracking function
double bestCost;		// Variables to hold the best found soultion during the runtime.
vector<vector<int>> bestAssign;

// Function to print the input data.
void printData(vector<Customer>& customers, vector<double>& vehicles, vector<vector<int>>& assign){
	cout << setprecision(6) << fixed;
	cout << "Customer data:" << endl;
	cout << "i: \t" << "Demand \t" <<  "X \t" << "Y" << endl;
	for(int i=0; i<customers.size(); i++){
		cout << i << ": \t";
		cout << customers[i].Demand << "\t";
		cout << customers[i].X << "\t";
		cout << customers[i].Y << endl;
	}
	cout << endl;
	
	cout << "Vehicles data:" << endl;
	cout << "i: \t" << "Capacity" << endl;
	for(int i=0; i<vehicles.size(); i++){
		cout << i << ": \t";
		cout << vehicles[i] << endl;
	}
	cout << endl;
	
	cout << "Assign data:" << endl;
	for(int i=0; i<assign.size(); i++){
		cout << i << ": \t";
		cout << "myAssign - ";
		for(int j=0; j<assign[i].size(); j++){
			cout << assign[i][j] << " ";
		}
		cout << "\t";
		cout << "bestAssign - ";
		for(int j=0; j<bestAssign[i].size(); j++){
			cout << bestAssign[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

// Function to print the assignment data passed through parameter.
// Also prints the best assignment on side.
void printAssign(vector<vector<int>>& assign){
	cout << "Assign data:" << endl;
	for(int i=0; i<assign.size(); i++){
		cout << i << ": \t";
		cout << "myAssign - ";
		for(int j=0; j<assign[i].size(); j++){
			cout << assign[i][j] << " ";
		}
		cout << "\t";
		cout << "bestAssign - ";
		for(int j=0; j<bestAssign[i].size(); j++){
			cout << bestAssign[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

// Function to calculate the distance between two locations.
// Uses pythagorus theorem for calculation.
double distance(double x1, double y1, double x2, double y2){
	double distance = sqrt(pow(x1-x2,2.0)+pow(y1-y2,2.0));
	return distance;
}

// Function to calculate the distance between customers using their index value.
// The index must be given in the data file output format. (In other words:
// Original index value '0' must be handed as value '1'.
double distanceBetween(vector<Customer>& customers, int i1, int i2){
	//i1 -= 1;
	//i2 -= 1;
	double x1, x2, y1, y2;
	if(i1 == -1){
		x1 = 0;
		y1 = 0;
		x2 = customers[i2].X;
		y2 = customers[i2].Y;
	}
	else if(i2 == -1){
		x1 = customers[i1].X;
		y1 = customers[i1].Y;
		x2 = 0;
		y2 = 0;
	}
	else{
		x1 = customers[i1].X;
		y1 = customers[i1].Y;
		x2 = customers[i2].X;
		y2 = customers[i2].Y;
	}
	return distance(x1, y1, x2, y2);
}

// Function to calculate the total cost of the given assignment.
double calculateScore(vector<Customer>& customers, vector<vector<int>>& assign){
	double score = 0;
	for(int i=0; i<assign.size(); i++){
		for(int j=1; j<assign[i].size(); j++){
			int c1, c2;
			c1 = assign[i][j-1];
			c2 = assign[i][j];
			
			score += distanceBetween(customers, c1, c2);
		}
	}
	return score;
}

// Function to find the closest customer from the given location, who is not already assigned.
int findClosestCustomertoLocUnderLimit(vector<Customer>& customers, vector<bool>& customerTaken, double limit, double x, double y){
	int closest = -1;
	double closestDist = numeric_limits<double>::infinity();
	for(int i=0; i<customers.size(); i++){
		if(!customerTaken[i] && customers[i].Demand < limit){
			double dist = distance(x, y, customers[i].X, customers[i].Y);
			if(dist < closestDist){
				closest = i;
				closestDist = dist;
			}
		}
	}
	return closest;
}

// Function to filter out the close living customers that can be legally assigned to a vehicle starting
// with given starting customer. Uses similar greedy solution idea of traveling salesman problem for doing this.
// Legal here means that the total demand of filter customers won't over occupy the vehicle's capacity.
void filterOutCloseCustomerStartingWithUnderLimit(vector<Customer>& customers, vector<bool>& customerTaken, double curCap, int startCust, vector<int>& filterCustomer){
	// Push the starting customer in our filter list.
	//filterCustomer.push_back(startCust);
	//customerTaken[startCust] = true;
	//curCap = curCap - customers[startCust].Demand;	// Update our capacity variable
	
	// Define our loop counters.
	double x = customers[startCust].X;
	double y = customers[startCust].Y;
	int c = findClosestCustomertoLocUnderLimit(customers, customerTaken, curCap, x, y);
	bool isDone = false;
	// Loop until we can fit our next customer in current vehicle or we can't find one.
	while(c != -1 && !isDone){
		if(customers[c].Demand < curCap){
			// If we are here means, we can fit customer 'c' in our vehicle.
			filterCustomer.push_back(c);	// Push 'c' into filtered list.
			customerTaken[c] = true;
			
			// And then update the parameters respectively for next iteration.
			curCap = curCap - customers[c].Demand;
			x = customers[c].X;
			y = customers[c].Y;
			c = findClosestCustomertoLocUnderLimit(customers, customerTaken, curCap, x, y);
		}
		else{
			// If we are here means we can't fit any customer in it anymore.
			isDone = true;
		}
	}
}

// Function to find the greedy solution to the problem.
// It uses the greedy approach of assigning closer customers to vehicles until.
// It automatically saves our best known solution in pre defined Global variables.
double findGreedySolution(vector<Customer>& customers, vector<bool>& customerTaken, vector<double>& vehicles){
	double ourCost = 0;		// Variable to hold the cost value.
	//vector<bool> customerTaken;
	//customerTaken.resize(customers.size(), false);
	//customerTaken[0] = true;
	
	// Initially use the for each customer algorithm to find the greedy solution.
	int numCustLeft = customers.size()-1;
	bool isDone = false;
	int v = 0;
	// We loop until we have assigned all the customers or we run out of vehicles.
	while(!isDone && v<vehicles.size()){
		vector<int> curCustIndList;	// vector to hold the filtered customer for current vehicle.
		
		// Find the closest not taken customer from the factory (starting customer for the tour)
		int c = 0;
		// Filter out the tour customers.
		filterOutCloseCustomerStartingWithUnderLimit(customers, customerTaken, vehicles[v], c, curCustIndList);
		
		// Then we assign them. (Directly storing it in bestAssign)
		for(int i=0; i<curCustIndList.size(); i++){
			bestAssign[v].push_back(curCustIndList[i]);
			numCustLeft -= 1;
		}
		bestAssign[v].push_back(0);	// At last add an extra '-1' because tour must end with -1.
		
		// If we have assigned all the customers, then we are done.
		if(numCustLeft <= 0){
			isDone = true;
		}
		
		// Counter increment.
		v++;
	}
	
	// Calculate the cost from your assignment, and save and return it.
	ourCost = calculateScore(customers, bestAssign);
	bestCost = ourCost;
	return ourCost;
}

// Function to find the solution to the vehicle routing problem using backtracking.
// Explore each and every possible solutions with backtracking using vehicles as index.
double vehicleRoutingSimulator(vector<Customer>& customers, vector<bool>& customerTaken, vector<double>& vehicles, vector<vector<int>>& curAssign, int numCustLeft, double curCost = 0, int curV = 0){
	//cout << "curCost : " << curCost << endl;
	//printAssign(curAssign);
	
	// If time difference exceeds the limit, then end the search.
	if(difftime(time(NULL), startTime) >= maxRunTime){
		return numeric_limits<double>::infinity();
	}
	
	// Base case: We found a solution.
	if(numCustLeft <= 0){
		// Now we add the return distance cost of each vehicle from the last customer in their tour.
		for(int i=0; i<curAssign.size(); i++){
			// If the size of tour is greater than 1, that means we are using that vehicle, else not.
			if(curAssign[i].size() > 1){
				// Update the cost, when vehicle is used.
				int i1 = curAssign[i][(curAssign[i].size()-1)];
				int i2 = 0;
				curCost += distanceBetween(customers, i1, i2);
			}
		}
		// If curCost is better than our best cost (if we have a best solution), than update our best found.
		if(curCost < bestCost || bestCost == 0){
			bestCost = curCost;
			for(int i=0; i<curAssign.size(); i++){
				bestAssign[i].resize(curAssign[i].size());
				for(int j=0; j<curAssign[i].size(); j++){
					bestAssign[i][j] = curAssign[i][j];
				}
				bestAssign[i].push_back(0);
			}
			//cout << "New Solution Found : " << curCost << endl;
			//printAssign(curAssign);
		}
		return curCost;
	}
	// Terminating case: We run out of vehicles, and our numCustLeft are not zero.
	// Which means we found an incorrect solution. So return infinity.
	else if(curV >= vehicles.size() && numCustLeft != 0){
		return numeric_limits<double>::infinity();
	}
	// Branch and Bound case: If our cost exceeds best found (if we have one), then stop branching.
	else if(bestCost > 0 && curCost >= bestCost){
		return curCost;
	}
	// Else: Keep exploring new solutions.
	else{
		// Try all the possible solution and select the one with least cost.
		double leastCost = numeric_limits<double>::infinity();
		
		// Loop counter variable. (Stars with '1' because '0' is the factory).
		int c = 1;
		// We loop unitl we have a valid customer index.
		while(c < customers.size()){
			// If the customer's demand can be fulfilled by the vehicle, then take it.
			if(vehicles[curV] >= customers[c].Demand && !customerTaken[c]){
			
				// Take the customer, and update the respective variables.
				customerTaken[c] = true;
				numCustLeft -= 1;
				curAssign[curV].push_back(c);
				vehicles[curV] -= customers[c].Demand;
				int prevC = curAssign[curV][(curAssign[curV].size()-2)];
				curCost = curCost + distanceBetween(customers, prevC, c);
				
				// Recursive call.
				double ourCost = vehicleRoutingSimulator(customers, customerTaken, vehicles, curAssign, numCustLeft, curCost, curV);
				// We want the solution with least cost, so update it accordingly.
				if(ourCost < leastCost)
					leastCost = ourCost;
				
				// Untake it and then try other possible paths. (Backtracking part)
				curCost = curCost - distanceBetween(customers, prevC, c);
				vehicles[curV] += customers[c].Demand;
				curAssign[curV].pop_back();
				numCustLeft += 1;
				customerTaken[c] = false;
			}
			c++;
		}
		// If we run out of customers, then start with the next vehicle.
		if(c >= customers.size()){
			double ourCost = vehicleRoutingSimulator(customers, customerTaken, vehicles, curAssign, numCustLeft, curCost, curV+1);
			// We want the solution with least cost, so update it accordingly.
			if(ourCost < leastCost)
				leastCost = ourCost;
		}
		// Return the solution with least cost.
		return leastCost;
	}
}

// Verifier to check whether our solution is valid or not.
// Directly uses the best found variables for verification.
bool verifier(vector<Customer>& customers, vector<bool>& customerTaken, vector<double>& vehicles, double cost, vector<vector<int>>& assign){
	// First check if our customer distribution is valid.
	vector<double> capacityOccupied;
	capacityOccupied.resize(vehicles.size(), 0);
	// Loop to calculate the occupied space of each vehicle.
	for(int i=0; i<assign.size(); i++){
		for(int j=1; j<assign[i].size()-1; j++){
			int c = assign[i][j];
			capacityOccupied[i] += customers[c].Demand;
			customerTaken[c] = true;
		}
	}
	
	bool isDistributionOk = true;
	for(int i=1; i<customerTaken.size(); i++){
		// See if any of the vehicle is over occupied or not.
		if(!customerTaken[i]){
			cout << "Customer " << i << " : Not Assigned" << endl;
			isDistributionOk = false;
		}		
	}
	// If all customers are assigned, then check the vehicle capacity distribution.
	if(isDistributionOk){
		for(int i=0; i<capacityOccupied.size(); i++){
			// See if any of the vehicle is over occupied or not.
			if(capacityOccupied[i] > vehicles[i]){
				cout << "Vehicle " << i << " : Occupied : " << capacityOccupied[i] << " || ";
				cout << "Capacity : " << vehicles[i] << endl;
				isDistributionOk = false;
			}		
		}
	}
	
	// If isDistributionOk stays true that means, our distribution is valid.
	if(isDistributionOk){
		// Now check if the calculated cost is correct or not.
		double verifyCost = calculateScore(customers, assign);
		
		cout << "Solution Cost = " << cost << " || Verifier Cost = " << verifyCost << endl;
		
		if(verifyCost == cost)
			return true;
		else
			return false;
	}
	return isDistributionOk;
}

int main(){
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Version: Asks user to enter the input file name and runs the program. 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Reading the file name and opening it
	string filename;
	cout << "Enter the filename : ";
	cin >> filename;
	string outputfile = filename + "_output";
	
	ifstream fin;
	ofstream fout;
	fin.open(filename.data());
	
	while (!fin.is_open())	{
		fin.clear();
		cout << "Invalid file, Enter a valid name : ";
		cin >> filename;
		fin.open(filename.data());
	}
	
	// Reading the content from file and saving it in corresponding variables.
	int numCustomers, numVehicles;
	double vehCapacity;
	fin >> numCustomers;
	fin >> numVehicles;
	fin >> vehCapacity;
	
	// Declaring the variables to hold the data.
	vector<Customer> myCustomers;		// Vector to hold all the customers data.
	myCustomers.resize(numCustomers);
	vector<bool> isCustomerTaken;
	isCustomerTaken.resize(numCustomers, false);
	isCustomerTaken[0] = true;
	vector<double> myVehicles;			// Vector to hold all the vehicles data.
	myVehicles.resize(numVehicles, vehCapacity);
	vector<vector<int>> myAssign;		// Vector to hold my current solution.
	myAssign.resize(numVehicles);
	bestAssign.resize(numVehicles);			// Vector to hold my best found solution.
	
	// Reading the file and storing the data.
	for(int i=0; i<numCustomers; i++){
		double temp;
		fin >> temp;
		myCustomers[i].Demand = temp;
		fin >> temp;
		myCustomers[i].X = temp;
		fin >> temp;
		myCustomers[i].Y = temp;
	}
	for(int i=0; i<numVehicles; i++){
		myAssign[i].resize(0);
		myAssign[i].push_back(0);
		bestAssign[i].resize(0);
		bestAssign[i].push_back(0);
	}
	
	//printData(myCustomers, myVehicles, myAssign);
	
	// Get the greedy solution and store it as our best solution.
	double greedy = findGreedySolution(myCustomers, isCustomerTaken, myVehicles);
	cout << "greedy = " << greedy << endl;
	
	for(int i=1; i<isCustomerTaken.size(); i++){
		isCustomerTaken[i] = false;
	}
	
	if(verifier(myCustomers, isCustomerTaken, myVehicles, bestCost, bestAssign)){
		cout << "======Valid Solution======" << endl;
		// Create the output file with the greedy output if greedy solution is verified.
		fout.open(outputfile.data(), ios_base::out);
		fout << bestCost << endl;
		for(int i=0; i<bestAssign.size(); i++){
			for(int j=0; j<bestAssign[i].size(); j++){
				fout << bestAssign[i][j] << " ";
			}
			fout << endl;
		}
		fout.close();		// Close the fout stream after printing the greedy solution.
	}
	else{
		cout << "=====Invalid Solution=====" << endl;
		// Greddy solution is incorrect, then reset best found solution.
		bestCost = 0;
		for(int i=0; i<bestAssign.size(); i++){
			bestAssign[i].resize(0);
			bestAssign[i].push_back(0);
		}
	}
	
	// Do the backtracking and see if you find a better solution
	for(int i=1; i<isCustomerTaken.size(); i++){
		isCustomerTaken[i] = false;
	}
	startTime = time(NULL);
	double backtracking = vehicleRoutingSimulator(myCustomers, isCustomerTaken, myVehicles, myAssign, (myCustomers.size()-1));
	cout << "backtracking = " << backtracking << endl;
	
	for(int i=1; i<isCustomerTaken.size(); i++){
		isCustomerTaken[i] = false;
	}
	
	if(verifier(myCustomers, isCustomerTaken, myVehicles, bestCost, bestAssign))
		cout << "======Valid Solution======" << endl;
	else
		cout << "=====Invalid Solution=====" << endl;
	
	cout << bestCost << endl;
	for(int i=0; i<bestAssign.size(); i++){
		for(int j=0; j<bestAssign[i].size(); j++){
			cout << bestAssign[i][j] << " ";
		}
		cout << endl;
	}
	
	// If our backtracking function don't crash, replace the file output with new solution.
	fout.open(outputfile.data(), ios_base::out);
	fout << bestCost << endl;
	for(int i=0; i<bestAssign.size(); i++){
		for(int j=0; j<bestAssign[i].size(); j++){
			fout << bestAssign[i][j] << " ";
		}
		fout << endl;
	}
	fout.close();
	
	return 0;
}