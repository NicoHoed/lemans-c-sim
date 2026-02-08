#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep()
#include "race.h"

// ANSI Color Codes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void print_status(RaceContext* race) {
    // Clear console
    printf("\033[H\033[J"); 

    // Time formatting
    int hours = (int)race->elapsed_time / 3600;
    int minutes = ((int)race->elapsed_time % 3600) / 60;
    int seconds = (int)race->elapsed_time % 60;

    printf("=== LE MANS 24H SIMULATION ===\n");
    printf("Race Time: %02dh %02dm %02ds\n\n", hours, minutes, seconds);
    
    // Header
    printf("%-4s | %-20s | %-10s | %-8s | %-12s | %-10s | %-6s\n", 
           "Pos", "Team", "Cat", "Laps", "Gap", "State", "Tires");
    printf("--------------------------------------------------------------------------------------\n");

    if (race->num_cars == 0) return;

    Car* leader = &race->cars[0];

    for (int i = 0; i < race->num_cars; i++) {
        Car* c = &race->cars[i];
        
        // 1. Determine Category Color & String
        char* cat_color = ANSI_COLOR_RESET;
        char cat_str[10];
        
        switch (c->category) {
            case LMH:
                cat_color = ANSI_COLOR_RED;
                sprintf(cat_str, "HYPER");
                break;
            case LMP2:
                cat_color = ANSI_COLOR_BLUE;
                sprintf(cat_str, "LMP2");
                break;
            case LMGT3:
                cat_color = ANSI_COLOR_YELLOW;
                sprintf(cat_str, "LMGT3");
                break;
        }

        // 2. Calculate Gap
        char gap_str[20];
        if (i == 0) {
            sprintf(gap_str, "LEADER");
        } else {
            int lap_diff = leader->laps_completed - c->laps_completed;
            if (lap_diff > 0) {
                sprintf(gap_str, "+%d Laps", lap_diff);
            } else {
                double time_diff = c->total_race_time - leader->total_race_time;
                sprintf(gap_str, "+%.1f s", time_diff);
            }
        }

        // 3. Determine State String (Racing or Pit)
        char state_str[10];
        char* state_color = ANSI_COLOR_GREEN;
        
        if (c->state == PIT_STOP) {
            sprintf(state_str, "IN PIT");
            state_color = ANSI_COLOR_MAGENTA; // Purple for Pit
        } else if (c->state == CRASHED) {
            sprintf(state_str, "CRASH");
            state_color = ANSI_COLOR_RED;
        } else {
            sprintf(state_str, "RUN");
        }

        // 4. Print the row
        // We print the category color, then the category text, then reset immediately
        printf("%-4d | %-20s | %s%-10s%s | %-8d | %-12s | %s%-10s%s | %.0f%%\n", 
               i + 1, 
               c->team_name, 
               cat_color, cat_str, ANSI_COLOR_RESET, // Colored Category
               c->laps_completed, 
               gap_str,
               state_color, state_str, ANSI_COLOR_RESET, // Colored State
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