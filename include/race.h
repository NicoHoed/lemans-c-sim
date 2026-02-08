#ifndef RACE_H
#define RACE_H

#include "car.h"

#define MAX_CARS 62 // Maximum grid size for Le Mans

typedef struct {
    Car *cars;              // Dynamic array of cars
    int num_cars;           // Current number of cars
    
    double elapsed_time;    // Time elapsed since race start (seconds)
    bool is_running;        // Simulation running flag
    
    // We can add weather here later
} RaceContext;

// Function Prototypes
void race_init(RaceContext* race, int num_cars_to_create);
void race_run_step(RaceContext* race);
void race_cleanup(RaceContext* race); // Free memory

#endif