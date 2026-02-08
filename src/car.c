#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // For logic (if needed later)
#include "car.h"
#include "core.h"

// Constants for simulation (approximate sector times in seconds)
#define BASE_TIME_LMH  38.0 // Fast sector
#define BASE_TIME_LMP2 41.0
#define BASE_TIME_GT3  46.0

// Fuel consumption per sector (liters)
#define FUEL_BURN_LMH  2.5
#define FUEL_BURN_LMP2 2.1
#define FUEL_BURN_GT3  1.8

// Tire wear per sector (percentage)
#define TIRE_WEAR_RATE 0.8 

void car_init(Car* car, int id, const char* team, const char* driver, CarCategory cat) {
    if (!car) return;
    car->id = id;
    strncpy(car->team_name, team, 63);
    strncpy(car->driver_name, driver, 63);
    car->category = cat;
    car->state = RACING;

    car->speed_kmh = 0.0;
    car->fuel_level = 100.0; 
    car->tire_wear = 0.0;

    car->current_lap_time = 0.0;
    car->last_lap_time = 0.0;
    car->total_race_time = 0.0;
    car->laps_completed = 0;
    car->current_sector = 0;
    car->has_pitted_this_lap = false;
    
    for(int i=0; i<3; i++) car->sector_times[i] = 0.0;
}

// Internal helper to get base time
static double get_base_sector_time(CarCategory cat) {
    switch (cat) {
        case LMH: return BASE_TIME_LMH;
        case LMP2: return BASE_TIME_LMP2;
        case LMGT3: return BASE_TIME_GT3;
        default: return 50.0;
    }
}

void car_update(Car* car, double delta_time) {
    (void)delta_time; // Not used in this specific logic version yet

    // 1. Calculate Sector Time
    double time = get_base_sector_time(car->category);
    
    // Add randomness (0.0 to 2.0 seconds) - traffic/driver skill
    double random_variation = (rand() % 200) / 100.0;
    time += random_variation;

    // Add tire wear penalty (e.g., if tires are 50% worn, add 1.5s)
    double wear_penalty = (car->tire_wear / 100.0) * 3.0; 
    time += wear_penalty;

    // 2. Consume Resources
    if (car->category == LMH) car->fuel_level -= FUEL_BURN_LMH;
    else if (car->category == LMP2) car->fuel_level -= FUEL_BURN_LMP2;
    else car->fuel_level -= FUEL_BURN_GT3;

    car->tire_wear += TIRE_WEAR_RATE + ((rand() % 50)/100.0); // Random wear

    // 3. Update Car Data
    car->sector_times[car->current_sector] = time;
    car->current_lap_time += time;
    car->total_race_time += time;

    // 4. Move to next sector
    car->current_sector++;

    // 5. Check for Lap Completion
    if (car->current_sector > 2) {
        car->laps_completed++;
        car->last_lap_time = car->current_lap_time;
        
        // Reset for next lap
        car->current_lap_time = 0.0;
        car->current_sector = 0;
        
        // Simple Pit Stop Logic (Reset if fuel low or tires dead)
        if (car->fuel_level < 10.0 || car->tire_wear > 80.0) {
            car->state = PIT_STOP;
            // Add pit stop time penalty (e.g., 40s)
            car->total_race_time += 40.0;
            car->last_lap_time += 40.0; // Pit is part of the lap
            
            // Refuel & New Tires
            car->fuel_level = 100.0;
            car->tire_wear = 0.0;
        } else {
            car->state = RACING;
        }
    }
}