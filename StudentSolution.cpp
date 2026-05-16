#include "acequia_manager.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

// Helper function to check if a region has excess water
double calculateExcess(Region* region)
{
   if (region->waterLevel > region->waterNeed && !region->isFlooded) {
      return region->waterLevel - region->waterNeed;
   }
   return 0.0;
}

// Helper function to calculate deficit
double calculateDeficit(Region* region)
{
   if (region->waterLevel < region->waterNeed && !region->isInDrought) {
      return region->waterNeed - region->waterLevel;
   }
   return 0.0;
}

// Helper function to get safe available water from source (don't drain below need)
double getSafeAmountToSend(Region* source, double requested)
{
   double excess = source->waterLevel - source->waterNeed;
   if (excess <= 0) return 0;
   return std::min(requested, excess);
}

// Helper function to get safe amount to receive (don't exceed capacity)
double getSafeAmountToReceive(Region* dest, double requested)
{
   double space = dest->waterCapacity - dest->waterLevel;
   if (space <= 0) return 0;
   return std::min(requested, space);
}

// Main solve function - Safe version that avoids penalties
void solveProblems(AcequiaManager& manager)
{
   auto canals = manager.getCanals();
   auto regions = manager.getRegions();

   std::cout << "\n=== Starting Water Management Solution (Safe Mode) ===" << std::endl;

   // Display initial state
   std::cout << "Initial State:" << std::endl;
   for (const auto& region : regions) {
      std::cout << "  " << region->name
                << " | Level: " << region->waterLevel
                << " | Need: " << region->waterNeed
                << " | Capacity: " << region->waterCapacity
                << " | Excess/Deficit: " << (region->waterLevel - region->waterNeed) << std::endl;
   }
   std::cout << std::endl;

   // Canal indices
   const int CANAL_A = 0;  // North -> South
   const int CANAL_B = 1;  // South -> East
   const int CANAL_C = 2;  // North -> East
   const int CANAL_D = 3;  // East -> North

   // Run until solved or time limit
   while (!manager.isSolved && manager.hour < manager.SimulationMax) {
      
      // Get current values
      double northLevel = regions[0]->waterLevel;
      double southLevel = regions[1]->waterLevel;
      double eastLevel = regions[2]->waterLevel;
      
      double northNeed = regions[0]->waterNeed;
      double southNeed = regions[1]->waterNeed;
      double eastNeed = regions[2]->waterNeed;
      
      double northCapacity = regions[0]->waterCapacity;
      double southCapacity = regions[1]->waterCapacity;
      double eastCapacity = regions[2]->waterCapacity;
      
      double northDeficit = calculateDeficit(regions[0]);
      double southDeficit = calculateDeficit(regions[1]);
      double eastDeficit = calculateDeficit(regions[2]);
      
      double northExcess = calculateExcess(regions[0]);
      double southExcess = calculateExcess(regions[1]);
      double eastExcess = calculateExcess(regions[2]);
      
      // Print status every 10 hours
      if (manager.hour % 10 == 0 && manager.hour > 0) {
         std::cout << "\n--- Hour " << manager.hour << " ---" << std::endl;
         std::cout << "North: " << std::fixed << std::setprecision(2) << northLevel 
                   << " (Need: " << northNeed << ", Cap: " << northCapacity << ")" << std::endl;
         std::cout << "South: " << southLevel 
                   << " (Need: " << southNeed << ", Cap: " << southCapacity << ")" << std::endl;
         std::cout << "East: " << eastLevel 
                   << " (Need: " << eastNeed << ", Cap: " << eastCapacity << ")" << std::endl;
      }
      
      // CLOSE ALL CANALS FIRST
      for (const auto& canal : canals) {
         if (canal->isOpen) {
            canal->toggleOpen(false);
            canal->setFlowRate(0.0);
         }
      }
      
      // Calculate safe flow rates based on deficits and excesses
      // Use lower flow rates to prevent overshoot (0.1 to 0.3 instead of 1.0)
      
      // Strategy 1: East has excess, send to North if needed
      if (eastExcess > 0.1 && northDeficit > 0.1) {
         double canSend = getSafeAmountToSend(regions[2], eastExcess);
         double canReceive = getSafeAmountToReceive(regions[0], northDeficit);
         double amount = std::min(canSend, canReceive);
         if (amount > 0) {
            double flowRate = std::min(0.3, amount / 10.0);
            canals[CANAL_D]->toggleOpen(true);
            canals[CANAL_D]->setFlowRate(flowRate);
            if (manager.hour % 10 == 0)
               std::cout << "  Opening Canal D (East->North) at flow " << flowRate << std::endl;
         }
      }
      
      // Strategy 2: North has excess, send to South if needed
      if (northExcess > 0.1 && southDeficit > 0.1) {
         double canSend = getSafeAmountToSend(regions[0], northExcess);
         double canReceive = getSafeAmountToReceive(regions[1], southDeficit);
         double amount = std::min(canSend, canReceive);
         if (amount > 0) {
            double flowRate = std::min(0.3, amount / 10.0);
            canals[CANAL_A]->toggleOpen(true);
            canals[CANAL_A]->setFlowRate(flowRate);
            if (manager.hour % 10 == 0)
               std::cout << "  Opening Canal A (North->South) at flow " << flowRate << std::endl;
         }
      }
      
      // Strategy 3: North has excess, send to East if needed
      if (northExcess > 0.1 && eastDeficit > 0.1) {
         double canSend = getSafeAmountToSend(regions[0], northExcess);
         double canReceive = getSafeAmountToReceive(regions[2], eastDeficit);
         double amount = std::min(canSend, canReceive);
         if (amount > 0) {
            double flowRate = std::min(0.3, amount / 10.0);
            canals[CANAL_C]->toggleOpen(true);
            canals[CANAL_C]->setFlowRate(flowRate);
            if (manager.hour % 10 == 0)
               std::cout << "  Opening Canal C (North->East) at flow " << flowRate << std::endl;
         }
      }
      
      // Strategy 4: South has excess, send to East if needed
      if (southExcess > 0.1 && eastDeficit > 0.1) {
         double canSend = getSafeAmountToSend(regions[1], southExcess);
         double canReceive = getSafeAmountToReceive(regions[2], eastDeficit);
         double amount = std::min(canSend, canReceive);
         if (amount > 0) {
            double flowRate = std::min(0.3, amount / 10.0);
            canals[CANAL_B]->toggleOpen(true);
            canals[CANAL_B]->setFlowRate(flowRate);
            if (manager.hour % 10 == 0)
               std::cout << "  Opening Canal B (South->East) at flow " << flowRate << std::endl;
         }
      }
      
      // Strategy 5: East has excess, send to South via North (only if both need)
      if (eastExcess > 0.1 && southDeficit > 0.1 && northDeficit < 0.1) {
         double canSend = getSafeAmountToSend(regions[2], eastExcess);
         double canReceive = getSafeAmountToReceive(regions[1], southDeficit);
         double amount = std::min(canSend, canReceive);
         if (amount > 0) {
            double flowRate = std::min(0.2, amount / 10.0);
            canals[CANAL_D]->toggleOpen(true);  // East -> North
            canals[CANAL_A]->toggleOpen(true);  // North -> South
            canals[CANAL_D]->setFlowRate(flowRate);
            canals[CANAL_A]->setFlowRate(flowRate);
            if (manager.hour % 10 == 0)
               std::cout << "  Opening path East->North->South at flow " << flowRate << std::endl;
         }
      }
      
      // Special case: If a region is very close to its need, use very low flow rate
      if (northDeficit > 0 && northDeficit < 1.0 && northDeficit > 0.01) {
         for (auto& canal : canals) {
            if (canal->destinationRegion == regions[0] && canal->isOpen) {
               canal->setFlowRate(0.05);
            }
         }
      }
      
      if (southDeficit > 0 && southDeficit < 1.0 && southDeficit > 0.01) {
         for (auto& canal : canals) {
            if (canal->destinationRegion == regions[1] && canal->isOpen) {
               canal->setFlowRate(0.05);
            }
         }
      }
      
      if (eastDeficit > 0 && eastDeficit < 1.0 && eastDeficit > 0.01) {
         for (auto& canal : canals) {
            if (canal->destinationRegion == regions[2] && canal->isOpen) {
               canal->setFlowRate(0.05);
            }
         }
      }
      
      // Advance to next hour
      manager.nexthour();
   }
   
   // Display final status
   std::cout << "\n=== Final Region Status ===" << std::endl;
   for (const auto& region : regions) {
      std::cout << "  " << region->name
                << " | Level: " << std::fixed << std::setprecision(2) << region->waterLevel
                << " | Need: " << region->waterNeed
                << " | Overflow Count: " << region->overflow
                << " | Drought Count: " << region->drought
                << std::endl;
   }
   
   // Check if solved
   bool solved = true;
   for (const auto& region : regions) {
      if (region->isFlooded || region->isInDrought || region->waterLevel < region->waterNeed) {
         solved = false;
         std::cout << region->name << " NOT satisfied" << std::endl;
      } else {
         std::cout << region->name << " SATISFIED!" << std::endl;
      }
   }
   
   if (solved) {
      std::cout << "\n*** ALL REGIONS SATISFIED! ***" << std::endl;
   }
   
   // Close all canals at the end
   for (const auto& canal : canals) {
      if (canal->isOpen) {
         canal->toggleOpen(false);
         canal->setFlowRate(0.0);
      }
   }
} 
