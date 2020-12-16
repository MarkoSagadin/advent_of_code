#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "utility.h"

FILE *f;

#define LEN 255

int get_answer_index(char letter);

int main()
{
    char line[LEN];
    bool group_ended = false;
    bool answers[26] = {0};
    int final_count = 0;

    //char letter = 'b';
    //int indexx = get_answer_index(letter);
    //printf("Letter %c gave index %d", letter, indexx);
    //return 0;


    /* Open file */
    f = fopen("input.txt","r");
    if (f == NULL) {
        printf("No file found!\n");
        return 0;
    }

    int num_lines = get_num_lines(f);
    int num_line = 0;

    /* Keep reading lines until you hit end */
    while (num_line < num_lines) {
        fgets(line, LEN, f);
        //printf("Line: %s", line);

        if (strcmp("\n", line) == 0) {
            group_ended = true;
        }
        

        if (!group_ended) {
            /* Remove first newline at the end of the string */
            int newline_pos = strlen(line) - 1;
            line[newline_pos] = '\0';

            /* Check each letter and raise its correct flag in array */
            int line_len = strlen(line);
            for (int i = 0; i < line_len; i++) {
                int index = get_answer_index(line[i]);
                answers[index] = true;
            }
        }

        if (group_ended || (num_line == (num_lines - 1))) {
            /* Count all yes questions */
            for (int i = 0; i < 26; i++) {
                if (answers[i]) final_count++;
            }
            /* reset answers */
            for (int i = 0; i < 26; i++) {
                answers[i] = false;
            }
            group_ended = false;
        }
        num_line++;
    }
    printf("Final count of correct answers was %d.", final_count);

    fclose(f);
    return 0;
}

int get_answer_index(char letter)
{
    /* a is 97 in ascii so to get index we simply convert it to int 
     * and subtract 97 */
    return (int)letter - 97;
}
