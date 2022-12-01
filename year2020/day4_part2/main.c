#include "utility.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *f;

/* example of input
hcl:#6b5442
pid:927492391
eyr:2023 hgt:172cm byr:1958 cid:92 ecl:gry iyr:2019

iyr:2020 cid:82
hgt:193in hcl:#b6652a
ecl:grn eyr:2034 byr:2026

iyr:1922 hcl:245cb3 byr:2015
pid:151cm
 */
#define LEN 255

bool special_check(char *cptr, int j);
struct passport {
	const char *field_strings[8];
	bool field_ids[8];
};

int main()
{
	char line[LEN];
	char *cptr;

	struct passport pass_info = {
		.field_strings = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid", "cid"},
	};

	bool passport_ended = false;
	int valid_pass_count = 0;

	/* Open file */
	f = fopen("input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	/* Keep reading lines until you hit end*/
	while (fgets(line, LEN, f) != NULL) {

		// printf("Line: %s", line);
		/* Check if we reached the end of passport */
		if (strcmp("\n", line) == 0) {
			passport_ended = true;
		}

		if (!passport_ended) {
			/* Remove first newline at the end of the string*/
			int newline_pos = strlen(line) - 1;
			line[newline_pos] = '\0';

			/* Split by space, lets have some dynamic fun*/
			/* Count number of delimiters*/
			int token_count = get_num_tokens_in_line(line, ' ');

			/* Create 2d array of strings */
			char **split_line = malloc(token_count * sizeof(char *));

			/* Start splitting them, for each token that you get
			 * save it into 2d array */
			cptr = strtok(line, " ");
			split_line[0] = (char *)malloc(strlen(cptr) + 1);
			strcpy(split_line[0], cptr);
			for (int i = 1; i < token_count; ++i) {
				cptr = strtok(NULL, " ");
				split_line[i] = (char *)malloc(strlen(cptr) + 1);
				strcpy(split_line[i], cptr);
			}

			/* Split each string by colon and compare it against
			 * struct of possible values*/
			/* for each value that you get, set a flag */
			for (int i = 0; i < token_count; ++i) {
				cptr = strtok(split_line[i], ":");
				for (int j = 0; j < 8; j++) {
					if (strcmp(pass_info.field_strings[j], cptr) == 0) {
						cptr = strtok(NULL, ":");
						pass_info.field_ids[j] = special_check(cptr, j);
						break;
					}
				}
			}

			/* Free space */
			for (int i = 0; i < token_count; i++)
				free(split_line[i]);
			free(split_line);
			/* go to next line */
		} else {
			/* in the end check that all flags are present one is optional, increase
			 * passport count */
			passport_ended = false;
			int flag_count = 0;
			for (int i = 0; i < 7; i++) {
				if (pass_info.field_ids[i]) {
					flag_count++;
				}
			}

			if (flag_count == 7) {
				valid_pass_count++;
			}
			/* Reset all flags */
			memset(pass_info.field_ids, 0, 8);
		}
	}

	/* Count for the last one that escaped */
	passport_ended = false;
	int flag_count = 0;
	for (int i = 0; i < 7; i++) {
		if (pass_info.field_ids[i]) {
			flag_count++;
		}
	}

	if (flag_count == 7) {
		valid_pass_count++;
	}
	/* Reset all flags */
	memset(pass_info.field_ids, 0, 8);
	printf("There were %d valid passports\n", valid_pass_count);
	fclose(f);
	return 0;
}

bool special_check(char *cptr, int j)
{
	switch (j) {
	case 0: {
		int num = atoi(cptr);
		if (1920 <= num && num <= 2002)
			return true;
	} break;
	case 1: {
		int num = atoi(cptr);
		if (2010 <= num && num <= 2020)
			return true;
	} break;
	case 2: {
		int num = atoi(cptr);
		if (2020 <= num && num <= 2030)
			return true;
	} break;
	case 3: {
		char *unit;
		int num = (int)strtol(cptr, &unit, 10);
		if (strcmp(unit, "cm") == 0) {
			if (150 <= num && num <= 193)
				return true;
		}
		if (strcmp(unit, "in") == 0) {
			if (59 <= num && num <= 76)
				return true;
		}

	} break;
	case 4:
		if (strncmp(cptr++, "#", 1) == 0) {
			if (strlen(cptr) == 6) {
				int tmp = (int)strtol(cptr, NULL, 16);
				if (!(tmp == 0 && errno == ERANGE))
					return true;
			}
		}
		break;
	case 5: {
		char *eye_colors[] = {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};
		for (int i = 0; i < 7; i++) {
			if (strcmp(eye_colors[i], cptr) == 0) {
				return true;
			}
		}
	} break;
	case 6:
		if (strlen(cptr) == 9) {
			long tmp = strtol(cptr, NULL, 10);
			if (!(tmp == 0 && errno == ERANGE))
				return true;
		}
		break;
	case 7:
		return true;
		break;
	}
	return false;
}
