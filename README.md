# Vehicle-Routing-Problem:
The Vehicle Routing Problem (VRP). Every day, a delivery company needs to deliver goods to many different customers. The deliveries are achieved by dispatching a fleet of vehicles from a centralized storage warehouse. The goal of this problem is to design a route for each vehicle (similar to traveling salesman tours) so that all of the customers are served by exactly one vehicle and the travel distance of the vehicles is minimized. Additional problem complexity comes from the fact that the vehicles have a fixed storage capacity and the customers have different demands.
The problem is mathematically formulated in the following way: We are given a list of locations N = 0 through n - 1. By convention, location 0 is the warehouse location, where all of the vehicles start and end their routes. The remaining locations are customers. Each location i from 1 to n-1 is characterized by three values: a demand di and a point (xi , yi). The fleet of vehicles V = 0 through v- 1 is fixed and each vehicle has the same limited capacity c. All of the demands assigned to a vehicle cannot exceed its capacity. For each vehicle i in V , let Ti be the sequence of customer deliveries made by that vehicle and let dist(c1, c2) be the Euclidean distance between two customers. Then the vehicle routing problem is formalized as the following optimization problem: Minimize the tour length of each vehicle such that the demand of all customers on each tour is ≤ c and each customer is in exactly one tour.
In this variant of the vehicle routing problem, we assume the vehicles can travel in straight lines between each pair of locations.

Input File Format:
The input consists of |N| + 1 lines. The first line contains 3 numbers: The number of customers |N|, the number of vehicles |V|, and the vehicle capacity c. It is followed by |N| lines, each line represents a location triple: di; xi; yii the demand and (x,y) location of each location.

Output File Format:
The output has |V |+1 lines. The first line contains the length of all of the vehicle routes (i.e. the objective value) as a real number. The following |V| lines represent the vehicle routes. Each vehicle line starts with warehouse identifier 0 followed by the identifiers of the customers serviced by that vehicle and ends with the warehouse identifier 0. Each vehicle line can contain between 2 and |N|+2 values depending on how many customers that vehicle services. Each customer identifier must appear in one of these vehicle lines.

An example:
Input Example
5 4 10
0 0 0 
3 0 10 
3 -10 10 
3 0 -10 
3 10 -10 
Output Example
80.6 
0 1 2 3 0 
0 4 0 
0 0 
0 0 
This output represents the following routes for each vehicle. Vehicle 0: 0->1->2->3->0. Vehicle 1: 0->4->0. Vehicle 2: 0->0 (not used), Vehicle 3: 0->0 (not used). Note the following equivalent solution using the same routes with different vehicles.
