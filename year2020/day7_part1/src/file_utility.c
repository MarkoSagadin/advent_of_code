/** @file file_utility.c
 * 
 * @brief Utility functions that operate on the given FILE stream.
 *
 */
#include "file_utility.h"

int get_num_lines(FILE *f)
{
    char c;
    int num_lines = 0;

    while(1) {
        c = getc(f);
        if (c == '\n') num_lines++;
        if (c == EOF) {
            fseek(f, 0, SEEK_SET);
            return num_lines;
        }
    }
}

int get_num_chars_in_line(FILE *f)
{
    char c;
    int num_chars = 0;

    while(1) {
        c = getc(f);

        if (c == '\n') {
            fseek(f, 0, SEEK_SET);
            return num_chars;
        }

        num_chars++;
    }
}

void copy_file_into_array(FILE *f, char * array)
{
    char c;
    while(1) {
        c = getc(f);

        if (c == EOF) {
            fseek(f, 0, SEEK_SET);
            break;
        }

        if (c != '\n') {
            *array++ = c;
        }
    }
}

int get_num_tokens_in_line(char * line, int delim)
{
    int delim_count = 0;
    char *cptr = line;
    while((cptr = strchr(cptr, delim)) != NULL) {
        delim_count++;
        cptr++;
    }
    return ++delim_count;
}

void move_to_nth_line(FILE *f, int nth_line)
{
    int num_of_newlines = 0;
    char c;
    while(1) {
        if (num_of_newlines == nth_line) {
            break;
        }
        c = getc(f);

        if (c == '\n') {
            num_of_newlines++;
        }
    }
}

int get_num_chars_in_nth_line(FILE *f, int nth_line)
{
    /* Set to the start of File, just in case. */
    rewind(f);

    /* Count number of newlines */
    move_to_nth_line(f, nth_line);

    int num_of_chars = get_num_chars_in_line(f) + 1;
    rewind(f);
    return num_of_chars;
}

char * get_nth_line(FILE *f, char *line, int max_line_len, int nth_file_line)
{
    /* Set to the start of File, just in case. */
    rewind(f);

    move_to_nth_line(f, nth_file_line);

    char * return_line = fgets(line, max_line_len, f);

    rewind(f);

    return return_line;
}

int get_len_sub_words(char *line, int start_word_index, int end_word_index)
{
    char c;
    int num_chars = 0;
    int num_detected_words = 0;

    do {
        c = *line;

        if (num_detected_words == end_word_index) {
            break;
        }

        if (c == ' ' || c == '\n') {
            num_detected_words++;
            /* To skip one silly case where num_chars would be unnecesary 
             * incremented. */ 
            if (start_word_index == num_detected_words) {
                continue;
            }
        }

        if (start_word_index <= num_detected_words &&
            end_word_index > num_detected_words ) {
            num_chars++;
        }
    } while(*line++);

    return num_chars;
}

void get_sub_words(char *line, char * words, int start_word_index, int end_word_index)
{
    char c;
    int num_chars = 0;
    int num_detected_words = 0;

    do {
        c = *line;

        if (num_detected_words == end_word_index) {
            break;
        }

        if (c == ' ' || c == '\n') {
            num_detected_words++;
            /* To skip one silly case where num_chars would be unnecesary 
             * incremented. */ 
            if (start_word_index == num_detected_words) {
                continue;
            }
        }

        if (start_word_index <= num_detected_words &&
            end_word_index > num_detected_words ) {
            *words++ = c;
        }
    } while(*line++);
    *words = '\0';
}

/*** end of file ***/
