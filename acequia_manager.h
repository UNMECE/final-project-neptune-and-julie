#ifndef ACEQUIA_MANAGER_H
#define ACEQUIA_MANAGER_H

#include <string>
#include <vector>
#include <map>

//Forward declarations
class Region;
class WaterSource;
class Canal;

//Enum for water source types
enum class WaterSourceType{
	RIVER,
	UNDERGROUND,
	DAM
};

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

		std::vector<WaterSource *> suppliedWater; //a container of all watersources that this region receives

		//default constructor
		Region (std::string name, double waterLevel, double waterNeed, double waterCapacity, bool isFlooded, bool isInDrought, int overflow, int drought);

		Region (std::string name, double waterLevel, double waterNeed, double waterCapacity);

		//function for changing the water level
		void updateWaterLevel(double change);
		//function for adding a new water source to a particular region
		void addWaterSource(WaterSource *source);
		//functions to update the boolean flags			
};
//WaterSource class representing a source of water and define the regions that a particular waterSource supplies to
class WaterSource{
	public:
		std::string name;
//		double volume; 		//perhaps adding a volume variable that changes water level during a transfer
		WaterSourceType type;
		double waterLevel; //current water level (acre-foot)
//		std::vector<Region *> suppliedRegions; //Regions supplied by this source
		WaterSource(std::string name, WaterSourceType type, double waterLevel);
//		void addSuppliedRegion(Region* region);
		
		//new functions for adding a list of canals that tap into this waterSource
//		std::vector<Canal *> canals; //the canals that move water from one region to another
//		void addCanal(Canal *canal);

		void updateWaterLevel(double change);
};

//Canal Class representing a canal connecting regions
class Canal{
	public:
		std::string name;
		Region* sourceRegion;
		Region* destinationRegion;
		WaterSource* waterSource;
		double flowRate; //Flow rate in gallons per second (0 to 1)
		bool isOpen; //is the canal open?

		Canal(std::string name, Region* sourceRegion, Region* destintionRegion, WaterSource* waterSource);
		void setFlowRate(double rate);
		void toggleOpen(bool open);
		void updateWater(int time);
};


/*New class added that can dynamically implement random weather conditions
Goal: to simulate weather cases at random times that alter the state of water levels to a particular region. 
We can achive this by:
 1. providing a string corresponding the weather event(intense heat, monsoon, light rain, snow fall)
 2. providing a method that alters water state (decreases water level or increases water level)
 3. asserts this action for a set rnage of time
 4. Informs a student that weather is 
*/
/*
class weather
{

}
*/

class WeatherEvents
{
	
};


//AcequiaManager class to manage the simulation
class AcequiaManager{
	private:
		std::vector<Region *> regions;
		std::vector<WaterSource *> waterSources;
		std::vector<Canal*> canals;
		std::map<std::string, double> leaderboard; //Leaderboard to rank solutions
		int InitDisparity;

//		std::stack<weather> weatherCases;

		int solvedTime; // total time for all region critera to be met
	public:
		int hour; 			//time in hours		
		int SimulationMax; //max time for the solution to be solved
		bool isSolved;
		

//Helper Function
		void initializeRegions();
		void initializeWaterSources();
		void initializeCanals();
		void initializeConstraints();
		void initializeTime();
		void initializeWeather();
		

		AcequiaManager();	//constructor
		~AcequiaManager();	//destructor


		void initializeRandomParameters();
		void displayState() const;
		void evaluateSolution();
//calculates any penalties during the simulation run
		int penalties();
		void displayLeaderboard() const;
//simulation run function
		void nexthour();
		bool solved();
		void nextHour();
		void step();
		void update();
		void tick();
		void simulateHour();
		void runHour();


		//Getters for students to access the simulation state
		const std::vector<Region *> &getRegions() const;
		const std::vector<WaterSource *> &getWaterSources() const;
		const std::vector<Canal *> &getCanals() const;
};

//Function to be implemented by students

void solveProblems(AcequiaManager& manager);

#endif
