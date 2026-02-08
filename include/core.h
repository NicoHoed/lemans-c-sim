#ifndef CORE_H
#define CORE_H

// Circuit: Circuit de la Sarthe (approximate lengths in km)
#define CIRCUIT_LENGTH_KM 13.626
#define SECTOR_1_LENGTH 4.3
#define SECTOR_2_LENGTH 5.1
#define SECTOR_3_LENGTH 4.2

// Time constants
#define SECONDS_IN_HOUR 3600
#define RACE_DURATION_HOURS 24
#define TOTAL_RACE_TIME (RACE_DURATION_HOURS * SECONDS_IN_HOUR)

// Simulation settings
// How many seconds pass in the simulation for one loop iteration?
// 1 = Real time, 60 = 1 minute per tick, etc.
#define SIMULATION_STEP_SECONDS 1 

#endif