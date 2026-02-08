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

// Updated function signature
void car_update(Car* car, double delta_time, bool is_safety_car) {
    
    (void)delta_time; // Mark as unused to avoid warnings for now

    // 0. Reset State if we were pitting
    if (car->state == PIT_STOP) {
        car->state = RACING;
    }

    // 1. Calculate Sector Time
    double time = get_base_sector_time(car->category);
    
    // SAFETY CAR LOGIC
    if (is_safety_car) {
        // Under SC, everyone drives slow (~80s per sector)
        time = 80.0 + ((rand() % 100) / 100.0); 
        
        car->fuel_level -= 0.2;
        car->tire_wear += 0.05; 
    } 
    else {
        // RACING LOGIC
        double random_variation = (rand() % 200) / 100.0; 
        double wear_penalty = (car->tire_wear / 100.0) * 3.0; 
        
        time += random_variation + wear_penalty;

        // Resource consumption
        if (car->category == LMH) car->fuel_level -= 2.5;
        else if (car->category == LMP2) car->fuel_level -= 2.1;
        else car->fuel_level -= 1.8;

        car->tire_wear += 0.8 + ((rand() % 50)/100.0);
    }

    // 2. Check for Pit Stop needs (Only if not already pitting)
    // Trigger if Fuel < 5L OR Tires > 85% worn
    if (car->state == RACING && (car->fuel_level < 5.0 || car->tire_wear > 85.0)) {
        car->state = PIT_STOP;
        
        // Add the Pit Penalty (approx 45s)
        time += 45.0;

        // Reset resources
        car->fuel_level = 100.0;
        car->tire_wear = 0.0;
        // Pense à ajouter 'int pit_stops_count' dans ta struct Car si tu veux compter les arrêts
    }

    // 3. Update Telemetry
    car->sector_times[car->current_sector] = time;
    car->current_lap_time += time;
    car->total_race_time += time;

    // 4. Move sector
    car->current_sector++;
    if (car->current_sector > 2) {
        car->laps_completed++;
        car->last_lap_time = car->current_lap_time;
        car->current_lap_time = 0.0;
        car->current_sector = 0;
    }
}