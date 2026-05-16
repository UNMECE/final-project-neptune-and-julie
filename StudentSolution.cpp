#include "acequia_manager.h"
#include <iostream>
#include <iomanip>

bool safeToSend(Region* src, Region* dst) {
    if (!src || !dst) return false;

    // Never drain a region in drought
    if (src->isInDrought) return false;

    // Never drain a region below its need
    if (src->waterLevel <= src->waterNeed) return false;

    // Never overflow destination
    if (dst->waterLevel >= dst->waterCapacity * 0.95) return false;

    return true;
}

void printStatus(Region* N, Region* S, Region* E, int hour) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n=== Hour " << hour << " ===\n";
    auto show = [](Region* r) {
        std::cout << r->name
                  << " | Level=" << r->waterLevel
                  << " Need="  << r->waterNeed
                  << " Cap="   << r->waterCapacity
                  << " | Drought=" << (r->isInDrought ? "YES" : "no")
                  << " Flood="   << (r->isFlooded ? "YES" : "no")
                  << "\n";
    };
    show(N); show(S); show(E);
}

void solveProblems(AcequiaManager& manager)
{
    auto R  = manager.getRegions();
    auto Cn = manager.getCanals();

    int n = R.size();
    Region* North = R[n - 3];
    Region* South = R[n - 2];
    Region* East  = R[n - 1];

    Canal* A = Cn[0]; // North -> South
    Canal* B = Cn[1]; // South -> East
    Canal* C = Cn[2]; // North -> East
    Canal* D = Cn[3]; // East  -> North

    bool phase1_done = false;

    while (!manager.isSolved && manager.hour < manager.SimulationMax)
    {
        A->toggleOpen(false);
        B->toggleOpen(false);
        C->toggleOpen(false);
        D->toggleOpen(false);

        printStatus(North, South, East, manager.hour);

        // ---------------- PHASE 1: Fill EAST ----------------
        if (!phase1_done) {
            if (East->waterLevel < East->waterNeed || East->isInDrought) {
                std::cout << "   [PHASE 1] Filling East\n";

                if (safeToSend(South, East)) {
                    B->setFlowRate(1.0);
                    B->toggleOpen(true);
                    std::cout << "   [ACTION] South -> East via B\n";
                }

                manager.nexthour();
                continue;
            }

            // East is solved → move to Phase 2
            phase1_done = true;
        }

        // ---------------- PHASE 2: Fill NORTH ----------------
        if (North->waterLevel < North->waterNeed) {
            std::cout << "   [PHASE 2] Filling North\n";

            bool moved = false;

            // Prefer South as donor
            if (safeToSend(South, North)) {
                A->setFlowRate(1.0);
                A->toggleOpen(true);
                std::cout << "   [ACTION] South -> North via A\n";
                moved = true;
            }

            // If South can't donate, let East help
            if (!moved && !East->isInDrought && East->waterLevel > East->waterNeed) {
                if (safeToSend(East, North)) {
                    D->setFlowRate(1.0);
                    D->toggleOpen(true);
                    std::cout << "   [ACTION] East -> North via D\n";
                }
            }

            manager.nexthour();
            continue;
        }

        manager.nexthour();
    }

    if (manager.isSolved) {
        std::cout << "\n>>> ALL REGIONS SOLVED at hour "
                  << manager.hour << " <<<\n";
    }
}
