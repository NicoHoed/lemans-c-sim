#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "race.h"

// --- 2025 ENTRY LIST DATA ---

typedef struct {
    const char* team;
    const char* driver;
    CarCategory category;
} RaceEntry;

// Extracted from your provided 2025 Entry List PDF
// Note: We use the first listed driver as the "Lead Driver"
static const RaceEntry REAL_ENTRIES[] = {
    // --- HYPERCAR ---
    {"Aston Martin Thor Team", "Harry Tincknell", LMH},
    {"Aston Martin Thor Team", "Alex Riberas",    LMH},
    {"Porsche Penske",         "Felipe Nasr",     LMH},
    {"Porsche Penske",         "Julien Andlauer", LMH},
    {"Porsche Penske",         "Kevin Estre",     LMH},
    {"Toyota Gazoo Racing",    "Mike Conway",     LMH},
    {"Toyota Gazoo Racing",    "Sebastien Buemi", LMH},
    {"Cadillac Hertz Jota",    "Will Stevens",    LMH},
    {"Cadillac Hertz Jota",    "Earl Bamber",     LMH},
    {"BMW M Team WRT",         "Dries Vanthoor",  LMH},
    {"BMW M Team WRT",         "Rene Rast",       LMH},
    {"Alpine Endurance",       "Paul-Loup Chatin",LMH},
    {"Alpine Endurance",       "Mick Schumacher", LMH},
    {"Ferrari AF Corse",       "Antonio Fuoco",   LMH},
    {"Ferrari AF Corse",       "A. Pier Guidi",   LMH},
    {"AF Corse (Yellow)",      "Robert Kubica",   LMH},
    {"Peugeot TotalEnergies",  "Paul Di Resta",   LMH},
    {"Peugeot TotalEnergies",  "Loic Duval",      LMH},
    {"Proton Competition",     "Neel Jani",       LMH},
    {"Cadillac WTR",           "Ricky Taylor",    LMH},
    {"Cadillac Whelen",        "Jack Aitken",     LMH},

    // --- LMP2 ---
    {"Iron Lynx Proton",       "Jonas Ried",      LMP2},
    {"Proton Competition",     "Giorgio Roda",    LMP2},
    {"United Autosports",      "R. van der Zande",LMP2},
    {"United Autosports",      "Daniel Schneider",LMP2},
    {"Inter Europol",          "Jakub Smiechowski",LMP2},
    {"IDEC Sport",             "Paul Lafargue",   LMP2},
    {"AO by TF",               "PJ Hyett",        LMP2},
    {"Algarve Pro Racing",     "Matthias Kaiser", LMP2},
    {"Vector Sport",           "Ryan Cullen",     LMP2}, // Placeholder for 'Persport' in PDF likely Vector/similar

    // --- LMGT3 ---
    {"Team WRT (BMW)",         "Valentino Rossi", LMGT3},
    {"Team WRT (BMW)",         "Yasser Shahin",   LMGT3},
    {"Iron Dames",             "Sarah Bovy",      LMGT3},
    {"Manthey PureRxcing",     "Antares Au",      LMGT3},
    {"Manthey EMA",            "Ryan Hardwick",   LMGT3},
    {"TF Sport (Corvette)",    "Tom Van Rompuy",  LMGT3},
    {"TF Sport (Corvette)",    "Hiroshi Koizumi", LMGT3},
    {"Vista AF Corse",         "Thomas Flohr",    LMGT3},
    {"Vista AF Corse",         "Francois Heriau", LMGT3},
    {"Heart of Racing",        "Ian James",       LMGT3},
    {"Proton (Mustang)",       "Ben Tuck",        LMGT3},
    {"Akkodis ASP (Lexus)",    "Arnold Robin",    LMGT3}
};

static const int TOTAL_ENTRIES = sizeof(REAL_ENTRIES) / sizeof(REAL_ENTRIES[0]);


// Helper function for qsort
int compare_cars(const void* a, const void* b) {
    Car* carA = (Car*)a;
    Car* carB = (Car*)b;

    // 1. Sort by Laps (Descending)
    if (carA->laps_completed > carB->laps_completed) return -1;
    if (carA->laps_completed < carB->laps_completed) return 1;

    // 2. Sort by Total Time (Ascending)
    if (carA->total_race_time < carB->total_race_time) return -1;
    if (carA->total_race_time > carB->total_race_time) return 1;

    return 0;
}

void race_init(RaceContext* race, int num_cars_to_create) {
    if (!race) return;

    // Cap the number of cars to our real data limit (or MAX_CARS)
    if (num_cars_to_create > TOTAL_ENTRIES) {
        num_cars_to_create = TOTAL_ENTRIES;
        printf("Note: Capped car count to %d (number of real entries defined).\n", TOTAL_ENTRIES);
    }
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
    race->is_running = false; 
    
    race->safety_car_active = false;
    race->safety_car_timer = 0;

    race->weather = WEATHER_SUNNY;
    race->weather_timer = 50; 

    srand(time(NULL));

    // --- POPULATE FROM REAL ENTRIES ---
    for (int i = 0; i < num_cars_to_create; i++) {
        const RaceEntry* entry = &REAL_ENTRIES[i];
        
        // Initialize the car with the specific data from our list
        car_init(&race->cars[i], i + 1, entry->team, entry->driver, entry->category);
    }
    
    printf("Race initialized with %d cars from 2025 Entry List.\n", num_cars_to_create);
}

void race_run_step(RaceContext* race) {
    if (!race || !race->cars) return;

    // --- 1. Manage Safety Car ---
    if (race->safety_car_active) {
        race->safety_car_timer--;
        if (race->safety_car_timer <= 0) {
            race->safety_car_active = false;
        }
    } else {
        if ((rand() % 100) < 1) { 
            race->safety_car_active = true;
            race->safety_car_timer = 5 + (rand() % 10);
        }
    }

    // --- 2. Manage Weather ---
    if (race->weather_timer > 0) {
        race->weather_timer--;
    } else {
        // Small chance to toggle weather (1%)
        if ((rand() % 100) < 1) {
            if (race->weather == WEATHER_SUNNY) {
                race->weather = WEATHER_RAIN;
                race->weather_timer = 100;
            } else {
                race->weather = WEATHER_SUNNY;
                race->weather_timer = 100;
            }
        }
    }

    // --- 3. Update each car ---
    for (int i = 0; i < race->num_cars; i++) {
        // Pass weather state (cast to int)
        car_update(&race->cars[i], 1.0, race->safety_car_active, (int)race->weather);
    }

    // --- 4. Sort the grid ---
    qsort(race->cars, race->num_cars, sizeof(Car), compare_cars);

    // --- 5. Update global race time ---
    race->elapsed_time += 40.0; 
}

void race_cleanup(RaceContext* race) {
    if (race && race->cars) {
        free(race->cars);
        race->cars = NULL;
        printf("Memory cleaned up.\n");
    }
}