#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

FILE *f;

#define LEN 255

int main()
{
    /* Open file */
    f = fopen("input.txt","r");
    if (f == NULL) {
        printf("No file found!\n");
        return 0;
    }

    char buff[LEN];
    int num_valid_pass = 0;
    char * bounds;
    char * letter;
    char * pass;

    int first_pos;
    int last_pos;

    while(1) {

        /* read each line */
        if (fgets(buff, LEN, f) == NULL) {
            printf("Number of valid passwords: %d", num_valid_pass);
            break;     
        }
  
        /* split it into 3 sections */
        bounds = strtok(buff, " ");
        letter = strtok(NULL, " ");
        pass = strtok(NULL, " ");
   
        /* get lower and upper boundary */
        first_pos = atoi(strtok(bounds, "-")) - 1;
        last_pos = atoi(strtok(NULL, "-")) - 1;

        /* check the password */
        if ( pass[first_pos] == *letter || pass[last_pos] == *letter) {
            if ( !(pass[first_pos] == *letter && pass[last_pos] == *letter)) {
                num_valid_pass++;
            }
        }
   }

    fclose(f);
    return 0;
}
