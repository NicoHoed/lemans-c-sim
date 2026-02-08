#ifndef CAR_H
#define CAR_H

#include <stdbool.h>

// Enumeration for car categories
typedef enum {
    LMH,    // Le Mans Hypercar
    LMP2,   // Le Mans Prototype 2
    LMGT3   // Le Mans GT3
} CarCategory;

// Enumeration for car state
typedef enum {
    RACING,
    PIT_STOP,
    CRASHED,
    RETIRED
} CarState;

// Tire Compounds
typedef enum {
    TIRE_SOFT,
    TIRE_MEDIUM,
    TIRE_HARD,
    TIRE_WET
} TireCompound;

typedef struct {
    int id;                 
    char team_name[64];
    char driver_name[64];   
    CarCategory category;
    
    // State
    CarState state;
    bool has_pitted_this_lap;

    // Performance & Telemetry
    double speed_kmh;       
    double fuel_level;      
    double tire_wear;       // 0.0 (new) to 100.0 (destroyed)
    
    // NEW: Current Tire
    TireCompound current_tires;

    // Timing (in seconds)
    double current_lap_time;
    double last_lap_time;
    double sector_times[3]; 
    double total_race_time; 
    
    int current_sector;     
    int laps_completed;

} Car;

// Function Prototypes
void car_init(Car* car, int id, const char* team, const char* driver, CarCategory cat);

// NEW: Update prototype to include weather_state
// weather_state: 0 = Sunny, 1 = Rain (mapped to enum in race.h)
void car_update(Car* car, double delta_time, bool is_safety_car, int weather_state);

#endif