#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "utility.h"

FILE *f;

#define LEN 255

int main()
{
    char line[LEN] = "BBFFBBFRLL";
    char * cptr;
    int low_row = 0;
    int high_row = 127;
    int low_column = 0;
    int high_column = 7;
    int highest_seat_id = 0;
    int current_seat_id = 0;
    bool taken_seats[128*8] = {0};

    /* Open file */
    f = fopen("input.txt","r");
    if (f == NULL) {
        printf("No file found!\n");
        return 0;
    }

    /* Keep reading lines until you hit end*/
    while (fgets(line, LEN, f) != NULL) {
        //printf("Line: %s", line);

        /* Remove first newline at the end of the string*/
        int newline_pos = strlen(line) - 1;
        line[newline_pos] = '\0';

        for (int i = 0; i < 7; i++) {
            int tmp = low_row + (high_row - low_row) / 2;
            if (line[i] == 'F') {
                high_row = tmp;
            }
            else if (line[i] == 'B') {
                low_row = tmp + 1;
            }
        }

        //printf("Low row is %d\n", low_row);
        //printf("High row is %d\n", high_row);

        for (int i = 7; i < 10; i++) {
            int tmp = low_column + (high_column - low_column) / 2;
            if (line[i] == 'L') {
                high_column = tmp;
            }
            else if (line[i] == 'R') {
                low_column = tmp + 1;
            }
        }

        //printf("Low column is %d\n", low_column);
        //printf("High column is %d\n", high_column);
        current_seat_id = low_row * 8 + low_column;

        taken_seats[current_seat_id] = true;
                    
        /* Reset */
        low_row = 0;
        high_row = 127;
        low_column = 0;
        high_column = 7;
    }

    for (int i = 1; i < (128 * 8) - 1; i++ ) {
        if (taken_seats[i] == false &&
            taken_seats[i-1] == true &&
            taken_seats[i+1] == true) {
        printf("Seat with id %d is free and yours to take\n", i);
        }
    }

    fclose(f);
    return 0;
}
