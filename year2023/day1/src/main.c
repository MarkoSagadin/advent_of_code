#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *f;

/**
 * @brief Do stuff
 *
 * @param ptr
 * @param increment
 * @return
 */
static int first_part_get_num(char *ptr, bool increment)
{
	int num;
	while (1) {
		if (isdigit(*ptr)) {
			num = (*ptr - '0');
			return num;
		}
		if (increment)
			ptr++;
		else
			ptr--;
	}
}
char *numbers[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
		   "0",	   "1",	  "2",	 "3",	  "4",	  "5",	  "6",	 "7",	  "8",	   "9"};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define MIN(a, b)     (a < b ? a : b)

static int second_part_get_num(char *ptr, bool increment)
{
	while (1) {

		for (size_t i = 0; i < ARRAY_SIZE(numbers); i++) {
			size_t n = MIN(strlen(ptr), strlen(numbers[i]));

			if (!strncmp(ptr, numbers[i], n)) {
				return (i % 10);
			}
		}
		if (increment)
			ptr++;
		else
			ptr--;
	}
}

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	size_t num_char = 1024;
	char *line = malloc(num_char);
	long sum1 = 0;
	long sum2 = 0;

	/* Steps
	 * 1. Find first digit in line
	 * 2. Find last digit in line
	 * 3. Add them to sum
	 * 4. Repeat
	 * */
	while (1) {
		if (getline(&line, &num_char, f) == -1)
			break;

		char *ptr = line;

		sum1 += first_part_get_num(ptr, true) * 10;
		ptr = &line[strlen(line) - 1];
		sum1 += first_part_get_num(ptr, false);

		ptr = line;
		sum2 += second_part_get_num(ptr, true) * 10;
		ptr = &line[strlen(line) - 1];
		sum2 += second_part_get_num(ptr, false);
	}

	printf("Sum1 is: %lu\n", sum1);
	printf("Sum2 is: %lu\n", sum2);
}
