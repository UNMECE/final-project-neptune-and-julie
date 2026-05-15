#include "acequia_manager.h"
#include <iostream>

/*Instructions for this problem:

	The intend of this project is to simulate water management conservation principles in the context of New Mexico

	In this simulation, there exists several Regions (North, South, etc.). Each region class includes both:
	- a given water level
	- a given water need
	- a indicator boolean for if the region is flooded
	- an indicator boolean for if the region is in drought

	With each region, there are given waterSources provided (perhaps a .dat file list each waterSource to  a region) 
	and certain waterSources have canals connected to them to deliver water across regions.

	Given the current state of the region, students wil be asked to utlize the canals that connect regions to
	develop the logic and algorithm for finding a solution. The simulation has a fixed time



	The student solution will be evaluated on the criteria that each region meets the following:
	- a given region is NOT flooded
	- a given region is NOT in drought
	- the region waterNeed does not exceed the region waterLevel 
*/

/*This will be how the solveProblems function is set up. The student may enter their on  */
/*
void solveProblems(AcequiaManager& manager)
{
	//the student can call the members of the canals object such as name of canal. sourceRegion, and destinationRegion
	//This could be helpful in informing the students strategy to solve the problem
	auto canals = manager.getCanals();
	//students may call to get Region and WaterSource objects to inform decisions 


	while(!manager.isSolved && manager.hour!=manager.SimulationMax)
	{	
		//enter student code here


		manager.nexthour();
	}
}
*/


/*example 1 format:

Setting particular commands to the canals at each time instance.

*/

/*
void solveProblems(AcequiaManager& manager)
{
	auto canals = manager.getCanals();
	while(!manager.isSolved && manager.hour!=manager.SimulationMax)
	{
	//Students will implement this function to solve the probelms
	//Example: Adjust canal flow rates and directions
		if(manager.hour==0)
		{
			canals[0]->setFlowRate(1);
			canals[0]->toggleOpen(true);
		}
		else if(manager.hour==1)
		{
			canals[1]->setFlowRate(0.5);
			canals[1]->toggleOpen(true);
		}
		else if(manager.hour==82)
		{
			canals[0]->toggleOpen(false);
			canals[1]->toggleOpen(false);
		}
	//student may add any necessary functions or check on the progress of each region as the simulation moves forward. 
	//The manager takes care of updating the waterLevels of each region and waterSource while the student is just expected
	//to solve how to address the state of each region

		
		manager.nexthour();
	}
}
*/

/*example 3:
Setting multiple canals at a particular time instance
*/

void solveProblems(AcequiaManager& manager)
{

auto canals = manager.getCanals();
auto regions = manager.getRegions();

// helper to get region by name such as north south etc

auto getRegion = [&](std::string name) {
	for (auto r : regions)
		if (r->name == name)
			return r;
		return regions[0]; // should never hit (backup)
};

Region* North = getRegion("North");
Region* South = getRegion("South");
Region* East = getRegion("East");

// canal mapping

Canal* A = canals[0]; // north to south
Canal* B = canals[1]; // south to east
Canal* C = canals[2]; // north to east
Canal* D = canals[3]; // east to north

while (!manager.isSolved && manager.hour < manager.SimulationMax)
{
	// compute deficits and surpluses
	double deficitN = North->waterNeed - North->waterLevel;
	double deficitS = South->waterNeed - South->waterLevel;
	double deficitE = East->waterNeed - East->waterLevel;

	double surplusN = North->waterLevel - North->waterNeed;
	double surplusS = South->waterLevel - South->waterNeed;
	double surplusE = East->waterLevel - East->waterNeed;

	if (deficitN <= 0 && deficitS <= 0 && deficitE <= 0)
		break;

	// identify region with largest deficit

	Region* dst = North;
	double maxDef = deficitN;

	if (deficitS > maxDef){
		dst = South;
		maxDef = deficitS;
}
	if (deficitE > maxDef){
		dst = East;
		maxDef = deficitE;
}

	// identify region with largest surplus

	Region* src = North;
	double maxSur = surplusN;

	if (surplusS > maxSur){
		src = South;
		maxSur = surplusS;
}
	if (surplusE > maxSur){
		src = East;
		maxSur = surplusE;
}

	// close all canals by default

	A->toggleOpen(false);
	B->toggleOpen(false);
	C->toggleOpen(false);
	D->toggleOpen(false);

// safety rules for the canals
	auto safeToSend = [&](Region* src, Region* dst){
	if (src->waterLevel < 0.20 * src->waterCapacity) return false; // avoid drought
	if (dst->waterLevel > 0.90 * dst->waterCapacity) return false; // avoid flood
	return true;
};

double rate = 0.5;

// Canal A

if (src == North && dst == South && safeToSend(North, South)){
	A->setFlowRate(rate);
	A->toggleOpen(true);
}

// Canal B

else if (src == South && dst == East && safeToSend(South, East)){
	B->setFlowRate(rate);
	B->toggleOpen(true);
}

// Canal C

else if (src == North && dst == East && safeToSend(North, East)){
	C->setFlowRate(rate);
	C->toggleOpen(true);
}

// Canal D

else if (src == East && dst == North && safeToSend(East, North)){
	D->setFlowRate(rate);
	D->toggleOpen(true);
}


// advance simulation

manager.nexthour();

manager.solved();
	}


// code to debug
std::cout << "\n\n ----------------------------------\n";
std::cout << "            FINAL SIMULATION STATE      \n";
std::cout << " ---------------------------------------\n\n";
std::cout << "Hour: " << manager.hour << "\n";
std::cout << "North:  Level=" << North->waterLevel
          << " Need=" << North->waterNeed
          << "Capacity=" << North->waterCapacity << "\n";

std::cout << "South:  Level=" << South->waterLevel
          << " Need=" << South->waterNeed
          << " Capacity=" << South->waterCapacity << "\n";

std::cout << "East:  Level=" << East->waterLevel
          << " Need=" << East->waterNeed
          << " Capacity=" << East->waterCapacity << "\n";

std::cout << "Canals: "
          << " A(" << A->isOpen << ")"
          << " B(" << B->isOpen << ")"
          << " C(" << C->isOpen << ")"
          << " D(" << D->isOpen << ")"
          << "\n------------------------------------\n";

}



/*Example 3 */
/*
void getProblemRegions(std::vector<Region *> regions, std::vector<std::string> &DefReg, std::vector<std::string> &GoodReg)
{
	//A function that finds which regions are in good standing and which are in bad standing
}

int getRegion(std::vector<Region *> regions, std::string name)
{
	//a function that gets the name of a region
}

std::string findCanal(std::string region, std::vector<Canal *> canals)
{
	//a function that finds a solution canal to solve the problem
}

void getRegionStatus(std::vector<Region *> regions)
{
	//a function that prints teh current status of a region
}

void solveProblems(AcequiaManager& manager)
{

	auto canals = manager.getCanals();
	auto regions = manager.getRegions();
	//finding the status of the region
	
	std::vector<std::string> ProbRegs;
	std::vector<std::string> GoodRegs;
	std::string targetCanal;
	while(!manager.isSolved && manager.hour!=manager.SimulationMax)
	{
		getRegionStatus(regions);
		getProblemRegions(regions, ProbRegs, GoodRegs);
		if(ProbRegs.size() != 0)
		{
				//find a canal that provides water to that region and adjust canal settings
	
		}
		if(GoodRegs.size() != 0)
		{
			//if a region is good, then leave then no need to open canals
		}

		manager.nexthour();
	}
}
*/
