#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <stdlib.h>

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

/* Ignore newline characters */
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

#endif /* UTILITY_H */
/*** end of file ***/
