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
#define ANSI_COLOR_GRAY    "\x1b[90m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void print_status(RaceContext* race) {
    printf("\033[H\033[J"); 

    int hours = (int)race->elapsed_time / 3600;
    int minutes = ((int)race->elapsed_time % 3600) / 60;
    int seconds = (int)race->elapsed_time % 60;

    printf("=== LE MANS 24H SIMULATION ===\n");
    
    // Weather
    if (race->weather == WEATHER_RAIN) {
         printf("Weather: 🌧️  %sRAIN / WET TRACK%s  Time: %02dh %02dm %02ds\n", 
                ANSI_COLOR_BLUE, ANSI_COLOR_RESET, hours, minutes, seconds);
    } else {
         printf("Weather: ☀️  %sSUNNY / DRY TRACK%s  Time: %02dh %02dm %02ds\n", 
                ANSI_COLOR_YELLOW, ANSI_COLOR_RESET, hours, minutes, seconds);
    }

    // Safety Car
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
           "Pos", "Team", "Cat", "Laps", "Gap", "State", "Tire", "Rel%");
    printf("--------------------------------------------------------------------------------------------------------\n");

    if (race->num_cars == 0) return;
    Car* leader = &race->cars[0];

    for (int i = 0; i < race->num_cars; i++) {
        Car* c = &race->cars[i];
        
        // NEW: Check if retired to override colors
        bool is_retired = (c->state == RETIRED);
        char* base_color = is_retired ? ANSI_COLOR_GRAY : ANSI_COLOR_RESET;

        // 1. Categories
        char* cat_color = ANSI_COLOR_RESET;
        char cat_str[10];
        
        if (is_retired) {
             cat_color = ANSI_COLOR_GRAY; // Grey out category too
        } else {
            if (c->category == LMH) cat_color = ANSI_COLOR_RED;
            else if (c->category == LMP2) cat_color = ANSI_COLOR_BLUE;
            else cat_color = ANSI_COLOR_YELLOW;
        }
        
        if (c->category == LMH) sprintf(cat_str, "HYPER");
        else if (c->category == LMP2) sprintf(cat_str, "LMP2");
        else sprintf(cat_str, "LMGT3");

        // 2. Gap
        char gap_str[20];
        if (is_retired) {
            sprintf(gap_str, "---");
        } else if (i == 0) {
            sprintf(gap_str, "LEADER");
        } else {
            int lap_diff = leader->laps_completed - c->laps_completed;
            if (lap_diff > 0) sprintf(gap_str, "+%d Laps", lap_diff);
            else sprintf(gap_str, "+%.1f s", c->total_race_time - leader->total_race_time);
        }

        // 3. State
        char state_str[10];
        char* state_color = ANSI_COLOR_GREEN;
        
        if (is_retired) {
            sprintf(state_str, "DNF");
            state_color = ANSI_COLOR_RED; // Keep DNF red for visibility, or make it Gray
        } else if (c->state == PIT_STOP) {
            sprintf(state_str, "IN PIT");
            state_color = ANSI_COLOR_MAGENTA; 
        } else if (c->state == CRASHED) {
            sprintf(state_str, "CRASH");
            state_color = ANSI_COLOR_RED;
        } else {
            sprintf(state_str, "RUN");
        }

        // 4. Tire
        char tire_str[10];
        char* tire_color = ANSI_COLOR_WHITE;
        
        if (is_retired) {
            tire_color = ANSI_COLOR_GRAY;
            sprintf(tire_str, "---");
        } else {
            switch(c->current_tires) {
                case TIRE_SOFT:   sprintf(tire_str, "(S)oft"); tire_color = ANSI_COLOR_RED; break;
                case TIRE_MEDIUM: sprintf(tire_str, "(M)edium"); tire_color = ANSI_COLOR_YELLOW; break;
                case TIRE_HARD:   sprintf(tire_str, "(H)ard"); tire_color = ANSI_COLOR_WHITE; break;
                case TIRE_WET:    sprintf(tire_str, "(W)et"); tire_color = ANSI_COLOR_BLUE; break;
            }
        }

        // Print Row
        // Notice we use 'base_color' for text that doesn't have specific highlighting
        printf("%s%-4d%s | %s%-20s%s | %s%-10s%s | %s%-8d%s | %s%-12s%s | %s%-10s%s | %s%-10s%s | %s%.0f%%%s\n", 
               base_color, i + 1, ANSI_COLOR_RESET,
               base_color, c->team_name, ANSI_COLOR_RESET,
               cat_color, cat_str, ANSI_COLOR_RESET, 
               base_color, c->laps_completed, ANSI_COLOR_RESET,
               base_color, gap_str, ANSI_COLOR_RESET,
               state_color, state_str, ANSI_COLOR_RESET,
               tire_color, tire_str, ANSI_COLOR_RESET,
               base_color, c->reliability, ANSI_COLOR_RESET);
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