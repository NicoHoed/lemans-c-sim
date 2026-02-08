#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    car->reliability = 100.0;
    
    // Start on Slick tires (Randomly Soft or Medium)
    car->current_tires = (rand() % 2 == 0) ? TIRE_SOFT : TIRE_MEDIUM;
    
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

void car_update(Car* car, double delta_time, bool is_safety_car, int weather_state) {
    (void)delta_time; 

    // NEW: Early Exit if Retired
    // If the car is out, it stops updating completely.
    if (car->state == RETIRED) {
        return;
    }

    // 0. Reset Pit State
    if (car->state == PIT_STOP) {
        car->state = RACING;
    }

    double time = get_base_sector_time(car->category);
    bool is_raining = (weather_state == 1); 

    // --- PHYSICS ENGINE ---

    if (is_safety_car) {
        time = 80.0 + ((rand() % 100) / 100.0);
        car->fuel_level -= 0.2;
        car->tire_wear += 0.05; 
        // Reliability stays stable under SC
    } 
    else {
        // ... (Keep existing Tire Physics Logic) ...
        double tire_perf_mod = 0.0;
        double wear_rate_mod = 1.0;
        
        // NEW: Dangerous Driving Flag
        bool dangerous_conditions = false;

        if (is_raining) {
            // RAIN PHYSICS
            if (car->current_tires == TIRE_WET) {
                tire_perf_mod = 5.0; 
                wear_rate_mod = 1.0;
            } else {
                // SLICKS IN RAIN
                tire_perf_mod = 25.0; 
                wear_rate_mod = 0.5;  
                dangerous_conditions = true; // High risk!
            }
        } else {
             // ... (Keep existing Dry Physics) ...
            if (car->current_tires == TIRE_SOFT) { tire_perf_mod = -0.5; wear_rate_mod = 1.2; }
            else if (car->current_tires == TIRE_MEDIUM) { tire_perf_mod = 0.0; wear_rate_mod = 1.0; }
            else if (car->current_tires == TIRE_HARD) { tire_perf_mod = 0.6; wear_rate_mod = 0.7; }
            else if (car->current_tires == TIRE_WET) { tire_perf_mod = 3.0; wear_rate_mod = 3.0; }
        }

        // Apply Time Logic
        double random_var = (rand() % 200) / 100.0; 
        double wear_penalty = (car->tire_wear / 100.0) * 4.0; 
        time += random_var + wear_penalty + tire_perf_mod;

        // Resources
        car->fuel_level -= 2.0; 
        car->tire_wear += (0.8 * wear_rate_mod) + ((rand() % 50)/100.0);
        
        // --- NEW: RELIABILITY LOGIC ---
        
        // 1. Base Decay (Hypercars are more fragile/high-strung)
        double decay = 0.0;
        if (car->category == LMH) decay = 0.05;
        else if (car->category == LMP2) decay = 0.03;
        else decay = 0.01; // GT3 are tanks
        
        // 2. Danger Penalty (Simulates crash risk)
        if (dangerous_conditions) {
            decay += 2.5; // Massive hit to reliability (represents probability of crash)
        }
        
        car->reliability -= decay;

        // 3. Catastrophic Failure (Random Event)
        // 0.01% chance per tick to blow an engine instantly
        if ((rand() % 10000) == 0) {
            car->reliability = -10.0; // Instant kill
        }
        
        // 4. Check Failure
        if (car->reliability <= 0.0) {
            car->state = RETIRED;
            // We do NOT update times, the car stops here.
            return; 
        }
    }

    // --- AI STRATEGY (PIT STOPS) ---
    // (Only if still running)
    
    bool need_pit = false;
    
    // 1. Check Resources
    if (car->fuel_level < 5.0 || car->tire_wear > 85.0) need_pit = true;

    // 2. Check Strategy (Wrong Tires)
    if (car->state == RACING) {
        if (is_raining && car->current_tires != TIRE_WET) need_pit = true;
        else if (!is_raining && car->current_tires == TIRE_WET) need_pit = true;
    }

    // Execute Pit Stop
    if (car->state == RACING && need_pit) {
        car->state = PIT_STOP;
        time += 45.0; 
        car->fuel_level = 100.0;
        car->tire_wear = 0.0;
        // Tire selection logic
        if (is_raining) car->current_tires = TIRE_WET;
        else {
            int r = rand() % 3;
            if (r == 0) car->current_tires = TIRE_SOFT;
            else if (r == 1) car->current_tires = TIRE_MEDIUM;
            else car->current_tires = TIRE_HARD;
        }
    }

    // --- TELEMETRY UPDATE ---
    car->sector_times[car->current_sector] = time;
    car->current_lap_time += time;
    car->total_race_time += time;

    car->current_sector++;
    if (car->current_sector > 2) {
        car->laps_completed++;
        car->last_lap_time = car->current_lap_time;
        car->current_lap_time = 0.0;
        car->current_sector = 0;
    }
}