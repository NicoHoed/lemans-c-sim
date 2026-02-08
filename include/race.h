#ifndef RACE_H
#define RACE_H

#include "car.h"

#define MAX_CARS 62 

// Weather States
typedef enum {
    WEATHER_SUNNY,
    WEATHER_RAIN
} WeatherState;

typedef struct {
    Car *cars;              
    int num_cars;           
    
    double elapsed_time;    
    bool is_running;        
    bool safety_car_active; 
    int safety_car_timer;    
    
    // Weather Context
    WeatherState weather;
    int weather_timer; // Prevents weather form changing too fast
    
} RaceContext;

// Function Prototypes
void race_init(RaceContext* race, int num_cars_to_create);
void race_run_step(RaceContext* race);
void race_cleanup(RaceContext* race); 

#endif