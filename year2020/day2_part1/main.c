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

    int low_bound;
    int high_bound;
    int letter_count = 0;

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
        low_bound = atoi(strtok(bounds, "-"));
        high_bound = atoi(strtok(NULL, "-"));
        
        /* check the password */
        for(int i = 0; pass[i] != '\0'; i++) {
            if (pass[i] == *letter) {
                letter_count++; 
            }
        }
        
        if (low_bound <= letter_count && letter_count <= high_bound) {
            num_valid_pass++;
        }
        letter_count = 0;
   }

    fclose(f);
    return 0;
}
