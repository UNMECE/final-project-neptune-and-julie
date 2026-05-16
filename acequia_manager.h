#ifndef ACEQUIA_MANAGER_H
#define ACEQUIA_MANAGER_H

#include <string>
#include <vector>
#include <map>

//Forward declarations
class Region;
class Canal;

//Region Class representing a region in New Mexico
class Region{
	public:
		std::string name;
		double waterLevel; //Current water level (acre-foot)
		double waterNeed; //Water needed by the region (acre-foot)
		double waterCapacity; //in acre-foot
		bool isFlooded; //is the region flooded?
		bool isInDrought;	//Is the region in drought?

		int overflow;
		int drought;

		//default constructor
		Region (std::string name, double waterLevel, double waterNeed, double waterCapacity, bool isFlooded, bool isInDrought, int overflow, int drought);

		Region (std::string name, double waterLevel, double waterNeed, double waterCapacity);

		//function for changing the water level
		void updateWaterLevel(double change);
};

//Canal Class representing a canal connecting regions
class Canal{
	public:
		std::string name;
		Region* sourceRegion;
		Region* destinationRegion;
		double flowRate; //Flow rate in gallons per second (0 to 1)
		bool isOpen; //is the canal open?

		Canal(std::string name, Region* sourceRegion, Region* destintionRegion);
		void setFlowRate(double rate);
		void toggleOpen(bool open);
		void updateWater(int time);
};

//AcequiaManager class to manage the simulation
class AcequiaManager{
	private:
		std::vector<Region *> regions;
		std::vector<Canal*> canals;
		int InitDisparity;
		int solvedTime; // total time for all region critera to be met

	public:
		int hour; 			//time in hours		
		int SimulationMax; //max time for the solution to be solved
		bool isSolved;
		
//Helper Function
		void initializeRegions();
		void initializeCanals();
		void initializeConstraints();
		void initializeTime();	

		AcequiaManager();	//constructor
		~AcequiaManager();	//destructor

		void initializeRandomParameters();
		void displayState() const;
		void evaluateSolution();
//calculates any penalties during the simulation run
		int penalties();
//simulation run function
		void nexthour();
		bool solved();

		//Getters for students to access the simulation state
		const std::vector<Region *> &getRegions() const;
		const std::vector<Canal *> &getCanals() const;
};

//Function to be implemented by students
void solveProblems(AcequiaManager& manager);

#endif
