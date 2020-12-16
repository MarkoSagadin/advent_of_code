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
    bool group_started = true;
    bool group_ended = false;
    bool answers[26] = {0};
    int final_count = 0;
    bool * answers_array;
    int persons_in_group = 0;

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

            /* Prepare for next person in a group */
            persons_in_group++;

            /* Allocate space for a new array of answers for this line */
            if (group_started) {
                answers_array = (bool *) calloc(26, sizeof(bool));
            }
            else {
                bool * new_answers_array = (bool *) 
                    realloc(answers_array, 26 * persons_in_group * sizeof(bool));
                if (new_answers_array)
                    answers_array = new_answers_array;
                else
                    printf("Not good");
            }
            group_started = false;

            // Clean newly created person
            for (int i = 0; i < 26; i++) {
                answers_array[i + 26 * (persons_in_group - 1)] = false;
            }

            /* Check each letter and raise its correct flag in array */
            int line_len = strlen(line);
            for (int i = 0; i < line_len; i++) {
                int index = get_answer_index(line[i]);
                answers_array[index + 26 * (persons_in_group - 1)] = true;
            }
        }

        if (group_ended || (num_line == (num_lines - 1))) {
            /* Count all yes questions, now we have to make sure that all 
             * persons in the group answered the question affirmative*/
            for (int i = 0; i < 26; i++) {
                int tmp_count = 0;

                for (int j = 0; j < persons_in_group; j++) {
                    if (answers_array[i + 26 * j]) tmp_count++;
                }
                /* Only increment final count if all persons confirmed */
                if (tmp_count == persons_in_group) final_count++;
            }


            /* reset answers */
            for (int i = 0; i < 26; i++) {
                for (int j = 0; j < persons_in_group; j++) {
                    answers_array[i + 26 * j] = false;
                }
            }
            group_ended = false;
            persons_in_group = 0;
            group_started = true;
            free(answers_array);
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
