#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "race.h"

// ANSI Color Codes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void print_status(RaceContext* race) {
    printf("\033[H\033[J"); 

    int hours = (int)race->elapsed_time / 3600;
    int minutes = ((int)race->elapsed_time % 3600) / 60;
    int seconds = (int)race->elapsed_time % 60;

    printf("=== LE MANS 24H SIMULATION ===\n");
    
    // --- WEATHER DISPLAY ---
    if (race->weather == WEATHER_RAIN) {
         printf("Weather: 🌧️  %sRAIN / WET TRACK%s  Time: %02dh %02dm %02ds\n", 
                ANSI_COLOR_BLUE, ANSI_COLOR_RESET, hours, minutes, seconds);
    } else {
         printf("Weather: ☀️  %sSUNNY / DRY TRACK%s  Time: %02dh %02dm %02ds\n", 
                ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, hours, minutes, seconds);
    }

    // --- SAFETY CAR ALERT ---
    if (race->safety_car_active) {
        printf("\n\033[43m\033[30m"); 
        printf("************************************************************************\n");
        printf("   SAFETY CAR DEPLOYED  -  NO OVERTAKING  -  SLOW DOWN  -  SC IN LAP   \n");
        printf("************************************************************************\n");
        printf("\033[0m\n"); 
    } else {
        printf("\nStatus: GREEN FLAG\n\n");
    }
    
    // Header
    printf("%-4s | %-20s | %-10s | %-8s | %-12s | %-10s | %-10s | %-6s\n", 
           "Pos", "Team", "Cat", "Laps", "Gap", "State", "Tire", "Wear");
    printf("-------------------------------------------------------------------------------------------------\n");

    if (race->num_cars == 0) return;
    Car* leader = &race->cars[0];

    for (int i = 0; i < race->num_cars; i++) {
        Car* c = &race->cars[i];
        
        // 1. Categories
        char* cat_color = ANSI_COLOR_RESET;
        char cat_str[10];
        if (c->category == LMH) { cat_color = ANSI_COLOR_RED; sprintf(cat_str, "HYPER"); }
        else if (c->category == LMP2) { cat_color = ANSI_COLOR_BLUE; sprintf(cat_str, "LMP2"); }
        else { cat_color = ANSI_COLOR_YELLOW; sprintf(cat_str, "LMGT3"); }

        // 2. Gap
        char gap_str[20];
        if (i == 0) sprintf(gap_str, "LEADER");
        else {
            int lap_diff = leader->laps_completed - c->laps_completed;
            if (lap_diff > 0) sprintf(gap_str, "+%d Laps", lap_diff);
            else sprintf(gap_str, "+%.1f s", c->total_race_time - leader->total_race_time);
        }

        // 3. State
        char state_str[10];
        char* state_color = ANSI_COLOR_GREEN;
        if (c->state == PIT_STOP) { sprintf(state_str, "IN PIT"); state_color = ANSI_COLOR_MAGENTA; }
        else if (c->state == CRASHED) { sprintf(state_str, "CRASH"); state_color = ANSI_COLOR_RED; }
        else sprintf(state_str, "RUN");

        // 4. NEW: Tire Display
        char tire_str[10];
        char* tire_color = ANSI_COLOR_WHITE;
        switch(c->current_tires) {
            case TIRE_SOFT:   sprintf(tire_str, "(S)oft"); tire_color = ANSI_COLOR_RED; break;
            case TIRE_MEDIUM: sprintf(tire_str, "(M)edium"); tire_color = ANSI_COLOR_YELLOW; break;
            case TIRE_HARD:   sprintf(tire_str, "(H)ard"); tire_color = ANSI_COLOR_WHITE; break;
            case TIRE_WET:    sprintf(tire_str, "(W)et"); tire_color = ANSI_COLOR_BLUE; break;
        }

        printf("%-4d | %-20s | %s%-10s%s | %-8d | %-12s | %s%-10s%s | %s%-10s%s | %.0f%%\n", 
               i + 1, c->team_name, 
               cat_color, cat_str, ANSI_COLOR_RESET, 
               c->laps_completed, gap_str,
               state_color, state_str, ANSI_COLOR_RESET,
               tire_color, tire_str, ANSI_COLOR_RESET,
               c->tire_wear);
    }
}

int main(void) {
    RaceContext race;
    race_init(&race, 10); // Create 10 cars

    printf("Starting Race...\n");
    sleep(1);

    // Simulation Loop (Simulate 50 steps = roughly 16 laps)
    for (int step = 0; step < 50; step++) {
        race_run_step(&race);
        print_status(&race);
        
        // Slow down the loop so we can watch it (100ms)
        usleep(100000); 
    }

    printf("\nSimulation Finished.\n");
    race_cleanup(&race);
    return 0;
}