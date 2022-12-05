/** @file file_utility.c
 *
 * @brief Utility functions that operate on the given FILE stream.
 *
 */
#include "file_utility.h"
#include <stdlib.h>

int get_num_lines(FILE *f)
{
	char c;
	int num_lines = 0;
	rewind(f);

	while (1) {
		c = getc(f);
		if (c == '\n') {
			num_lines++;
		}
		if (c == EOF) {
			break;
		}
	}
	rewind(f);
	return num_lines;
}

int get_num_chars_in_line(FILE *f)
{
	char c;
	int num_chars = 0;

	while (1) {
		c = getc(f);

		if (c == '\n') {
			fseek(f, 0, SEEK_SET);
			return num_chars;
		}

		num_chars++;
	}
}

void copy_file_into_array(FILE *f, char *array)
{
	char c;
	while (1) {
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

int get_num_tokens_in_line(char *line, int delim)
{
	int delim_count = 0;
	char *cptr = line;
	while ((cptr = strchr(cptr, delim)) != NULL) {
		delim_count++;
		cptr++;
	}
	return ++delim_count;
}

void move_to_nth_line(FILE *f, int nth_line)
{
	int num_of_newlines = 0;
	char c;
	while (1) {
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

char *get_nth_line(FILE *f, char *line, int max_line_len, int nth_line)
{
	/* Set to the start of File, just in case. */
	rewind(f);

	move_to_nth_line(f, nth_line);
	char *return_line = fgets(line, max_line_len, f);
	rewind(f);

	return return_line;
}

/* Helper function as functions get_len_sub_words and get_sub_words require
 * same loop logic. */
static int prv_sub_words_proc(char *line, char *sub_words, int start, int end)
{
	char c;
	int num_detected_words = 0;
	int num_chars = 0;

	do {
		c = *line;

		if (num_detected_words == end) {
			break;
		}

		if (c == ' ' || c == '\n') {
			num_detected_words++;
			/* To skip one silly case where num_chars would be unnecesary
			 * incremented. */
			if (start == num_detected_words) {
				continue;
			}
		}

		if (start <= num_detected_words && end > num_detected_words) {
			if (sub_words) {
				*sub_words++ = c;
			}
			num_chars++;
		}
	} while (*line++);

	if (sub_words) {
		*sub_words = '\0';
	}
	return num_chars;
}

int get_len_sub_words(char *line, int start, int end)
{
	return prv_sub_words_proc(line, NULL, start, end);
}

void get_sub_words(char *line, char *sub_words, int start, int end)
{
	prv_sub_words_proc(line, sub_words, start, end);
}

char *get_sub_words_in_nth_line(FILE *f, int nth_line, int start, int end)
{
	int len = get_num_chars_in_nth_line(f, nth_line) + 1;
	char *line = malloc(len * sizeof(char));

	get_nth_line(f, line, len, nth_line);

	len = get_len_sub_words(line, start, end) + 1;
	char *sub_words = malloc(len * sizeof(char));

	get_sub_words(line, sub_words, start, end);

	free(line);
	return sub_words;
}

/*** end of file ***/
