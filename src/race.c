#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "race.h"

// Helper arrays to generate realistic dummy data
static const char* LMH_TEAMS[] = {"Ferrari AF Corse", "Toyota Gazoo Racing", "Porsche Penske", "Cadillac Racing", "Peugeot TotalEnergies"};
static const char* LMP2_TEAMS[] = {"United Autosports", "Team WRT", "Alpine Elf Team", "Vector Sport"};
static const char* GT3_TEAMS[] = {"Iron Dames", "WRT BMW", "Manthey PureRxcing", "TF Sport Corvette"};

static const char* DRIVERS[] = {"A. Fuoco", "S. Buemi", "K. Estre", "E. Bamber", "L. Vanthoor", "Sarah Bovy", "V. Rossi"};

// Initialize the race context and generate the grid
void race_init(RaceContext* race, int num_cars_to_create) {
    if (!race) return;

    if (num_cars_to_create > MAX_CARS) {
        num_cars_to_create = MAX_CARS;
    }

    // Allocate memory for the cars
    race->cars = (Car*)malloc(num_cars_to_create * sizeof(Car));
    if (!race->cars) {
        fprintf(stderr, "Error: Failed to allocate memory for cars.\n");
        exit(EXIT_FAILURE);
    }

    race->num_cars = num_cars_to_create;
    race->elapsed_time = 0.0;
    race->is_running = false; // Waiting for start command

    // Seed the random number generator
    srand(time(NULL));

    // Populate the grid
    for (int i = 0; i < num_cars_to_create; i++) {
        CarCategory cat;
        const char* team;
        
        // Distribute categories roughly: 20% Hypercar, 30% LMP2, 50% GT3
        int rand_cat = rand() % 100;
        if (rand_cat < 20) {
            cat = LMH;
            team = LMH_TEAMS[rand() % 5];
        } else if (rand_cat < 50) {
            cat = LMP2;
            team = LMP2_TEAMS[rand() % 4];
        } else {
            cat = LMGT3;
            team = GT3_TEAMS[rand() % 4];
        }

        const char* driver = DRIVERS[rand() % 7];
        
        // Initialize the car (ID starts at 1)
        car_init(&race->cars[i], i + 1, team, driver, cat);
    }
    
    printf("Race initialized with %d cars.\n", num_cars_to_create);
}

void race_cleanup(RaceContext* race) {
    if (race && race->cars) {
        free(race->cars);
        race->cars = NULL;
        printf("Memory cleaned up.\n");
    }
}

// Helper function for qsort
// Returns -1 if A is better (ahead), 1 if B is better, 0 if equal
int compare_cars(const void* a, const void* b) {
    Car* carA = (Car*)a;
    Car* carB = (Car*)b;

    // 1. Sort by Laps (Descending)
    if (carA->laps_completed > carB->laps_completed) return -1;
    if (carA->laps_completed < carB->laps_completed) return 1;

    // 2. Sort by Total Time (Ascending) - smaller time means they finished the lap earlier
    if (carA->total_race_time < carB->total_race_time) return -1;
    if (carA->total_race_time > carB->total_race_time) return 1;

    return 0;
}

void race_run_step(RaceContext* race) {
    if (!race || !race->cars) return;
    
    // 2. Update each car
    for (int i = 0; i < race->num_cars; i++) {
        // Pass BOTH delta_time and safety_car status
        car_update(&race->cars[i], 1.0, race->safety_car_active);
    }

    // 2. Sort the grid to determine positions
    qsort(race->cars, race->num_cars, sizeof(Car), compare_cars);

    // 3. Update global race time (approximate)
    race->elapsed_time += 40.0; 
}