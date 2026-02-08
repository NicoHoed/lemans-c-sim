#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep()
#include "race.h"

void print_status(RaceContext* race) {
    // Clear console
    printf("\033[H\033[J"); 
    
    // Convert seconds to HH:MM:SS format
    int hours = (int)race->elapsed_time / 3600;
    int minutes = ((int)race->elapsed_time % 3600) / 60;
    int seconds = (int)race->elapsed_time % 60;

    printf("=== LE MANS 24H SIMULATION ===\n");
    printf("Race Time: %02dh %02dm %02ds\n\n", hours, minutes, seconds);
    
    printf("%-3s | %-4s | %-20s | %-8s | %-8s | %-12s | %-6s\n", 
           "Pos", "#", "Team", "Cat", "Laps", "Gap", "Tires");
    printf("------------------------------------------------------------------------------\n");

    // Get leader's data for gap calculation
    Car* leader = &race->cars[0];

    for (int i = 0; i < race->num_cars; i++) {
        Car* c = &race->cars[i];
        
        // Format Category
        char cat_str[10];
        if(c->category == LMH) sprintf(cat_str, "HYPER");
        else if(c->category == LMP2) sprintf(cat_str, "LMP2");
        else sprintf(cat_str, "GT3");

        // Format Gap
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

        // Color coding for tires (ANSI escape codes)
        // Green if > 70%, Yellow if > 40%, Red if < 40% (inverted for wear: 0 is new)
        char* color = "";
        if (c->tire_wear < 30.0) color = "\033[0;32m"; // Green (New)
        else if (c->tire_wear < 70.0) color = "\033[0;33m"; // Yellow (Used)
        else color = "\033[0;31m"; // Red (Worn)
        
        printf("%-3d | #%-3d | %-20s | %-8s | %-8d | %-12s | %s%.0f%%%s\n", 
               i + 1, c->id, c->team_name, cat_str, c->laps_completed, gap_str, 
               color, c->tire_wear, "\033[0m");
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