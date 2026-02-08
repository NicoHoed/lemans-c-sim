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

    // 0. Reset Pit State
    if (car->state == PIT_STOP) {
        car->state = RACING;
    }

    double time = get_base_sector_time(car->category);
    bool is_raining = (weather_state == 1); // 1 = WEATHER_RAIN

    // --- PHYSICS ENGINE ---

    if (is_safety_car) {
        time = 80.0 + ((rand() % 100) / 100.0);
        car->fuel_level -= 0.2;
        car->tire_wear += 0.05; 
    } 
    else {
        // 1. Tire Performance Modifiers
        double tire_perf_mod = 0.0;
        double wear_rate_mod = 1.0;

        if (is_raining) {
            // RAIN PHYSICS
            if (car->current_tires == TIRE_WET) {
                // Wets in rain = Normal speed but slower than dry track
                tire_perf_mod = 5.0; // Rain is naturally slower
                wear_rate_mod = 1.0;
            } else {
                // SLICKS IN RAIN = DISASTER
                tire_perf_mod = 25.0; // Massive penalty (sliding everywhere)
                wear_rate_mod = 0.5;  // Sliding doesn't wear tires as much as grip
            }
        } else {
            // DRY PHYSICS
            if (car->current_tires == TIRE_SOFT) {
                tire_perf_mod = -0.5; // Fast
                wear_rate_mod = 1.2;  // High wear
            } else if (car->current_tires == TIRE_MEDIUM) {
                tire_perf_mod = 0.0;  // Baseline
                wear_rate_mod = 1.0;
            } else if (car->current_tires == TIRE_HARD) {
                tire_perf_mod = 0.6;  // Slow
                wear_rate_mod = 0.7;  // Low wear
            } else if (car->current_tires == TIRE_WET) {
                // WETS IN DRY = OVERHEAT
                tire_perf_mod = 3.0;  // Slow
                wear_rate_mod = 3.0;  // Destroyed instantly
            }
        }

        // Apply Logic
        double random_var = (rand() % 200) / 100.0; 
        double wear_penalty = (car->tire_wear / 100.0) * 4.0; // Wear slows you down
        
        time += random_var + wear_penalty + tire_perf_mod;

        // Resource Consumption
        // (Simplified fuel burn)
        car->fuel_level -= 2.0; 
        car->tire_wear += (0.8 * wear_rate_mod) + ((rand() % 50)/100.0);
    }

    // --- AI STRATEGY (PIT STOPS) ---
    
    bool need_pit = false;
    
    // 1. Check Resources
    if (car->fuel_level < 5.0 || car->tire_wear > 85.0) need_pit = true;

    // 2. Check Strategy (Wrong Tires)
    if (car->state == RACING) {
        if (is_raining && car->current_tires != TIRE_WET) {
            need_pit = true; // FORCE PIT: It's raining and we are on slicks!
        }
        else if (!is_raining && car->current_tires == TIRE_WET) {
            need_pit = true; // FORCE PIT: Dry track and we are melting wet tires!
        }
    }

    // Execute Pit Stop
    if (car->state == RACING && need_pit) {
        car->state = PIT_STOP;
        time += 45.0; // Pit Penalty

        // Refuel & Reset Wear
        car->fuel_level = 100.0;
        car->tire_wear = 0.0;

        // SELECT NEW TIRES
        if (is_raining) {
            car->current_tires = TIRE_WET;
        } else {
            // In dry, choose Soft or Medium randomly for variety
            // Or choose Hard if it's night (future feature)
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