
#include <iostream>
#include <vector>
#include <map>
#include "acequia_manager.h"
#include <random>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

int main(int argc, char *argv[]){

	//setting the random number generator
	if(argc == 1)
	{

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> timeRandom(50,120);

	std::uniform_real_distribution<double> waterLevelRandom(20,100);
	std::uniform_real_distribution<double> waterNeedRandom(50,100);
	std::uniform_real_distribution<double> waterCapacityRandom(100,200);



	std::ofstream MyFile("RandomValues.dat");			

	std::ostringstream ss;

	int time = timeRandom(gen);
	ss <<"Max Simulation Time" <<std::endl;
	ss <<time <<std::endl;

	int regionWaterLevel;
	int regionWaterNeed;
	int regionWaterCapacity;
	std::string RegionName;
	ss<<"Random Values" <<std::endl;

	int waterDifference=0;
	int totalwaterLevel=0;
	int totalWaterNeed=0;
	std::cout<<"Current State of the Regions: " <<std::endl;
	std::cout<<"------------------------------" <<std::endl;
	for(int i = 0; i < 3;i++)	
	{
		regionWaterLevel = waterLevelRandom(gen);
		regionWaterNeed = waterNeedRandom(gen);
		regionWaterCapacity = waterCapacityRandom(gen);
		waterDifference = (regionWaterNeed - regionWaterLevel);
		std::uniform_real_distribution<double> waterLevelUpdate(waterDifference,100);
		if(i == 0)
		{
			RegionName = "North";
			while(waterDifference > 25)
			{

				regionWaterLevel = waterLevelUpdate(gen);
				waterDifference = (regionWaterNeed- regionWaterLevel);
			}	
		}
		else if(i == 1)
		{
			RegionName = "South";
			while(waterDifference > 25)
			{	
				regionWaterLevel = waterLevelUpdate(gen);
				waterDifference = (regionWaterNeed - regionWaterLevel);
			}

		}
		else if(i == 2)
		{
			RegionName = "East";
			waterDifference = (totalWaterNeed - totalwaterLevel) + regionWaterNeed;
//		std::cout<<"Water Difference: " <<waterDifference <<std::endl;
			if(waterDifference > 0)
			{
				std::uniform_real_distribution<double> waterLevelFinal(0, 10);
				regionWaterLevel = waterDifference + waterLevelFinal(gen);
				while(regionWaterCapacity < regionWaterLevel)
				{
					regionWaterCapacity = waterCapacityRandom(gen);
				}
			}
		}
		else
		{
			RegionName = "Unknown";
		}
		totalwaterLevel += regionWaterLevel;
		totalWaterNeed += regionWaterNeed;
		std::cout <<"Region: "<<RegionName <<", Water Level: "<<regionWaterLevel <<", Water Need: " <<regionWaterNeed <<", Water Capacity: " <<regionWaterCapacity	<<std::endl;
		ss <<RegionName <<"," <<regionWaterLevel <<"," <<regionWaterNeed <<"," <<regionWaterCapacity <<std::endl; 
	}
	std::cout<<"------------------------------------------------------------" <<std::endl;
	std::cout<<"Please write your solution in the StudentSolution.cpp file." <<std::endl;
	std::cout<<"Your code must solve each region's water needs within the following simulation time: " <<time <<std::endl;
	std::cout<<"When you have saved your code and ready to run the simulation:" <<std::endl;
	std::cout<<"1. Recompile using the following command: g++ -o sim_test SimulatorMain.cpp AcequiaManager.cpp StudentSolution.cpp -std=C==1" <<std::endl;
	std::cout<<"2. Run the execute (For this example, the command will be: ./sim_test.exe run)" <<std::endl;
	
	MyFile<<ss.str();				
	MyFile.close();	
	return 0;
	}	
	//Initialize the Acequia Manger with Random Parameters
	AcequiaManager manager;
	manager.initializeRandomParameters();

	//Display initial state
//	manager.displayState();

 	//Students will implement this function to solve the problems
 	solveProblems(manager);
 	manager.displayState();
 	//Evaluate the solution and display results
 	manager.evaluateSolution();
 	//manager.displayLeaderboard();
	

	return 0;

}
