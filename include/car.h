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

typedef struct {
    int id;                 // Car number (e.g., #51)
    char team_name[64];
    char driver_name[64];   // Current driver name
    CarCategory category;
    
    // State
    CarState state;
    bool has_pitted_this_lap;

    // Performance & Telemetry
    double speed_kmh;       // Current speed (for display mainly)
    double fuel_level;      // In liters
    double tire_wear;       // 0.0 (new) to 1.0 (destroyed)

    // Timing (in seconds)
    double current_lap_time;
    double last_lap_time;
    double sector_times[3]; // [0]=S1, [1]=S2, [2]=S3
    double total_race_time; // Total time since start
    
    int current_sector;     // 0, 1, or 2
    int laps_completed;

} Car;

// Function Prototypes
// Initializes a car with default values
void car_init(Car* car, int id, const char* team, const char* driver, CarCategory cat);

// Updates the car state for one simulation step
void car_update(Car* car, double delta_time);

#endif