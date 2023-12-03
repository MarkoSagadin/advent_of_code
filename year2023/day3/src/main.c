#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *f;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

char **read_lines(FILE *f, size_t *num_lines)
{
	char **lines = NULL;

	size_t count = 0;

	while (1) {
		char *line = NULL;
		size_t num_char;
		if (getline(&line, &num_char, f) == -1) {
			break;
		}

		lines = reallocarray(lines, count + 1, sizeof(char *));
		if (!lines) {
			printf("reallocarray failed");
			return NULL;
		}
		lines[count++] = line;
	}

	*num_lines = count;

	return lines;
}

/* Parse line in lines[row] until you hit a digit. Use strtol to get num, bloom_start and
 * bloom_end.
 * Return True, until end of line is reached, otherwise false. */
bool get_next_num(char **lines, int row, int *num, int *bloom_start, int *bloom_end)
{
	static int idx = 0;

	/* Subtract 1 for the newline. */
	char *line = lines[row];
	size_t line_len = strlen(line) - 1;

	while (idx < line_len) {
		/* Skip non-numbers */
		if (line[idx] < '0' || '9' < line[idx]) {
			idx++;
			continue;
		}

		char *endptr;
		*num = strtol(line + idx, &endptr, 10);

		*bloom_start = idx - 1;

		/* Some pointer arithmetic to get the length of the number in chars. */
		size_t len = endptr - (line + idx);

		*bloom_end = idx + len;

		/* Make index jump to the end of the number */
		idx = *bloom_end;
		return true;
	}

	/* Reset idx for the next line. */
	idx = 0;
	return false;
}

/* Scan inside the bloom, as dictated by the row, bloom_start and bloom_end for any non-digit,
 * non-period, non-newline character. If found return True. Ignore negative and out of bounds
 * numbers. */
bool is_part_num(char **lines, int num_lines, int row, int bloom_start, int bloom_end)
{
	/* Subtract 1 for the newline. */
	size_t line_len = strlen(lines[row]) - 1;

	for (int i = bloom_start; i <= bloom_end; i++) {
		for (int j = row - 1; j <= row + 1; j++) {
			/* Ignore negative and out of bounds indexes. */
			if (i < 0 || i >= line_len || j < 0 || j >= num_lines) {
				continue;
			}

			char c = lines[j][i];

			/* Skip numbers and dots */
			if (('0' <= c && c <= '9') || '.' == c || '\n' == c) {
				continue;
			}

			/* Found a special symbol. */
			return true;
		}
	}

	return false;
}

bool get_next_asterix(char **lines, int row, int *col)
{
	static int idx = 0;

	/* Subtract 1 for the newline. */
	char *line = lines[row];
	size_t line_len = strlen(line) - 1;

	while (idx < line_len) {
		/* Skip non-numbers */
		if (line[idx] != '*') {
			idx++;
			continue;
		}

		*col = idx++;
		return true;
	}

	/* Reset idx for the next line. */
	idx = 0;
	return false;
}

char *convert_into_number(char **lines, int row, int col, int *num)
{
	while (1) {
		char c = lines[row][col];
		if ('0' <= c && c <= '9') {

			if (--col >= 0)
				continue;
		}
		char *ptr = &lines[row][col + 1];
		*num = strtol(ptr, NULL, 10);
		return ptr;
	}
}
struct num_ptr_pair {
	int num;
	char *ptr;
};

long get_gear_ratio(char **lines, size_t num_lines, int row, int col)
{
	struct num_ptr_pair pairs[8] = {0};
	int num_found_pair = 0;

	/* Subtract 1 for the newline. */
	size_t line_len = strlen(lines[row]) - 1;

	for (int i = col - 1; i <= col + 1; i++) {
		for (int j = row - 1; j <= row + 1; j++) {
			/* Ignore negative and out of bounds indexes. */
			if (i < 0 || i >= line_len || j < 0 || j >= num_lines) {
				continue;
			}

			char c = lines[j][i];
			/* Skip non-numbers */
			if ('0' > c || c > '9') {
				continue;
			}

			int num;
			char *ptr = convert_into_number(lines, j, i, &num);

			if (num_found_pair == 0) {
				pairs[num_found_pair].ptr = ptr;
				pairs[num_found_pair].num = num;
				num_found_pair++;
				continue;
			}

			/* Check if there is already an existing number in array, if not store it.
			 */
			bool match = false;
			for (int k = 0; k < num_found_pair; k++) {
				if (pairs[k].ptr == ptr) {
					match = true;
				}
			}

			if (!match) {
				pairs[num_found_pair].ptr = ptr;
				pairs[num_found_pair].num = num;
				num_found_pair++;
			}
		}
	}

	return num_found_pair == 2 ? pairs[0].num * pairs[1].num : 0;
}

long first_part(char **lines, size_t num_lines)
{
	long sum = 0;
	int bloom_start, bloom_end, num;

	for (int row = 0; row < num_lines; row++) {
		/* Keep calling get_next_num until you reach the end of the line. */
		while (get_next_num(lines, row, &num, &bloom_start, &bloom_end)) {
			/* Check if there is a special character in the bloom. */
			// printf("\n\nline: %s", lines[row]);
			// printf("num: %i, bloom_start: %i, bloom_end: %i\n", num, bloom_start,
			//        bloom_end);

			if (is_part_num(lines, num_lines, row, bloom_start, bloom_end)) {
				sum += num;
			}
		}
	}

	return sum;
}

/*
 * Plan for second part:
 * For each line move index for char until you hit a '*'.
 * Scan its neighbours for a digit, when found determine the ptr start of the digit and the digit
 * itself.
 * Save both in a limited array of ptr-num struct pairs and keep scanning neighbours.
 * Each number you find you must compare its ptr against the stored ones, if there is a match drop
 * the ptr and the number, otherwise store it.
 *
 * After you are done scanning neighbours, check that you have exactly two numbers, multiply them to
 * get the gear ratio.
 * */
long second_part(char **lines, size_t num_lines)
{
	long sum = 0;
	int col;

	for (int row = 0; row < num_lines; row++) {
		/* Keep calling get_next_num until you reach the end of the line. */
		while (get_next_asterix(lines, row, &col)) {
			sum += get_gear_ratio(lines, num_lines, row, col);
		}
	}

	return sum;
}

/* Plan for first part:
 * For each line move index for char until you hit a number.
 * Use strtol to convert a number. endptr will give you the first non number.
 * Create a "bloom" around the number and inside of that "bloom" check for non-digit, non-period
 * characters.
 * */
int main()
{
	f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	size_t num_lines = 0;

	/* You should load entire file into a 2d array, it will make it easier to look into lines
	 * above and below you.
	 */
	char **lines = read_lines(f, &num_lines);

	long sum1 = first_part(lines, num_lines);
	long sum2 = second_part(lines, num_lines);

	printf("Part 1: %lu\n", sum1);
	printf("Part 2: %lu\n", sum2);
}
