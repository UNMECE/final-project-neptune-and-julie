#include "acequia_manager.h"
#include <iostream>
#include <random>
#include <fstream>
#include <string>
#include <sstream>
#include <queue>

using namespace std;

//IMPLEMENTING REGIONS ------------------------------
//constructors
Region::Region(string name, double waterLevel, double waterNeed, double waterCapacity, bool isFlooded, bool isInDrought, int overflow, int drought)
	: name(name), waterLevel(waterLevel), waterNeed(waterNeed), waterCapacity(waterCapacity), isFlooded(isFlooded), isInDrought(isInDrought), overflow(overflow), drought(drought){}

Region::Region (std::string name, double waterLevel, double waterNeed, double waterCapacity) { updateWaterLevel(0); };

//function adjusts the changes to the waterLevel. This can be a negative for positive change.
	void Region::updateWaterLevel(double change){
		waterLevel += change;
		if(waterLevel >= waterCapacity){ 	//condition for when waterLevel reaches waterCapacity
			waterLevel =waterCapacity;
			isFlooded= true;
			isInDrought = false;
			overflow++;
			std::cout<<"Overflow in region, "<<name <<". Count increasing. Count: " <<overflow <<std::end;
		}
		else if(waterLevel < waterCapacity && waterLevel>waterNeed)
		{
			isFlooded = false;
			isInDrought= false;
		}
		else if(waterLevel >= 0.2*waterCapacity)
		{
			isFlooded = false;
			isInDrought = false;
		}
		else if(waterLevel <= 0.2*waterCapacity){	//conditions for when waterLevel reaches drought levels
			isInDrought = true;
			isFlooded = false;
			drought++;
		}
		if(waterLevel < 0){		//conditions for when waterLevel is empty
			waterLevel=0;
			isInDrought = true;
			isFlooded = false;
		}
	}

//IMPLEMENTING CANALS --------------------------------------
//constructor
Canal::Canal(std::string name, Region* sourceRegion, Region* destinationRegion)
	:name(name), sourceRegion(sourceRegion), destinationRegion(destinationRegion), flowRate(0.0), isOpen(false){}

//setting a flow rate for a canal object
void Canal::setFlowRate(double rate) { flowRate = rate; }
//setting a canal as open for water to flow
void Canal::toggleOpen(bool open){ isOpen = open; }

//this function updates the water exchange between the regions and waterSources only if the canal is open. 
void Canal::updateWater(int time)	//seconds
{
	if(!isOpen) return;
   if(flowRate>1) return;

	double change = 0;			//change is the amount in gallons being moved
	for(int i = 0; i<time; i++) change += flowRate;
	double amount;
	amount = change/10000;			// current conversion to allow for readable change during the simulation
//	amount = change/325851;			//conversion to acre-foots
	sourceRegion->updateWaterLevel(-amount);		//this changes the waterLevel of the source region
	destinationRegion->updateWaterLevel(amount);	//this changes the waterLevel of the Destination region					
}

//IMPLEMENTATION ACEQUIA MANAGER --------------------------------------
//constructor and destructor
AcequiaManager::AcequiaManager(){}
AcequiaManager::~AcequiaManager(){
	for(auto region : regions)
		delete region;
	for(auto canal : canals)
		delete canal;
}
//initializing the Random parameters. using helper functions to initialize conditions
void AcequiaManager::initializeRandomParameters(){
	initializeRegions();
	initializeCanals();
	initializeConstraints();
	initializeTime();
}

//initializing the time settings
void AcequiaManager::initializeTime()
{
	hour = 0;
	solvedTime=0;
	isSolved = false;
}

void AcequiaManager::initializeRegions() {
      regions.push_back(new Region("North", 53.0, 49.5, 55,));
      regions.push_back(new Region("South", 40.0, 39, 45));
      regions.push_back(new Region("East", 25, 26, 50));
}

void AcequiaManager::initializeCanals(){
	//Example Canals
		canals.push_back(new Canal("Canal A", regions[0], regions[1]);
		canals.push_back(new Canal("Canal B", regions[1], regions[2]);
		canals.push_back(new Canal("Canal C", regions[0], regions[2]);
		canals.push_back(new Canal("Canal D", regions[2], regions[0]);		
}

void AcequiaManager::initializeConstraints(){
	//changing the boolean values for each region based on the randomly generated variables
	for(const auto& region:regions)
	{
		region->updateWaterLevel(0);
		region->overflow = 0;
		region->drought = 0;
	}
}

/*This function moves time forward while also brokering the water transfers that have been taken by the student. */
void AcequiaManager::nexthour(){
	for(const auto& canal: canals){
		if(canal->isOpen == true)
		{
			canal->updateWater(3600);
		}
	}
	//this checks to see if the criteria for being solved has been met.
	isSolved = solved();
	//if simulation is solved, the duration to solve is stored and the simulation ends
	if(isSolved == true)
	{
		solvedTime = hour;
	}
	hour++;
}



//this function checks to see if each region meets all the criteria and returns a boolean 
//if any criteria is NOT met, the boolean returns as false
//otherwise, the boolean will return as true
bool AcequiaManager::solved()
{
	bool test = true;
	for(const auto& region: regions)
	{
		if(region->isFlooded || region->isInDrought || region->waterLevel < region->waterNeed){
			test=false;
		}
	}
	return test;
}

//a function that counts every instance a region has been over capacity or completely droughted
int AcequiaManager::penalties(){
	int count = 0;

	for(const auto& region: regions)
	{
		count += region->overflow;
		count += region->drought; 
	}
	return count;
}

//showing the current state of each ergion
void AcequiaManager::displayState()const{
	std::cout<<"Current State: \n";
	std::cout<<"-----------------\n";
	for(const auto& region: regions){
		std::cout<<"Region: " <<region->name <<", Water Level: " <<region->waterLevel <<", Water Need: " <<region->waterNeed
		<<", Flooded: " <<(region->isFlooded?"Yes" : "No")
		<<", Drought: " <<(region->isInDrought?"Yes" : "No") <<"\n";
	}
	std::cout<<"------------------\n";
}

//evaluating the state of each region after the simulation has completed its run
void AcequiaManager::evaluateSolution(){
	//Evaluate the soulution based on how well the problems are addressed
	double score = 0.0;
	for(const auto& region: regions){
		if(!region->isFlooded && !region->isInDrought &&region->waterLevel >= region->waterNeed){
			score += 10.0;
		}
	}
	/*penalties calculation*/
	int penalty = 0;
	penalty = penalties();
	score -= penalty;

	//checking if the simulation was solved
	if(isSolved)		//if solved, report the time for solved
	{
		score += 50;	//a point bonus for solving in time
		std::cout<<"Time solved = " <<solvedTime <<std::endl;

	}
	else if(!isSolved)
	{					//if not solved, let the user know 
		std::cout<<"Not all regions were solved in time." <<std::endl;
	}



	//reporting the final score
	leaderboard["StudentSolution"] = score;
	std::cout<<"--------------------\n" <<std::endl;
}

const std::vector<Region *> & AcequiaManager::getRegions() const{
	return regions;
}

const std::vector<Canal*> &AcequiaManager::getCanals() const{
	return canals;
}

