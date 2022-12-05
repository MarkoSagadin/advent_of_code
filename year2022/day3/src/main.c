#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *f;

char find_common(char *one, char *two, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		for (size_t j = 0; j < len; j++) {
			if (one[i] == two[j])
				return one[i];
		}
	}
	return 0;
}

char find_common_in_three(char *x, char *y, char *z)
{
	for (size_t i = 0; i < strlen(x); i++) {
		for (size_t j = 0; j < strlen(y); j++) {
			for (size_t k = 0; k < strlen(z); k++) {
				if (x[i] == y[j] && y[j] == z[k])
					return x[i];
			}
		}
	}
	return 0;
}

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	/* Steps for first part
	 * 1. Split in two strings
	 * 2. Find common char in both
	 * 3. Covert char to priority value
	 * 4. Add together the priority values into sum
	 * */
	size_t num_char = 128;
	char *line = malloc(num_char);
	int answer1 = 0;
	/* Steps for second part
	 * 1. Store three strings
	 * 2. Find common char in all three
	 * 3. Covert char to priority value
	 * 4. Add together the priority values into sum
	 * */
	char lines[3][128];
	int idx = 0;
	int answer2 = 0;

	while (getline(&line, &num_char, f) != -1) {

		/* sub 1 cause of newline */
		size_t len = (strlen(line) - 1) / 2;
		char c = find_common(line, &line[len], len);
		/* 3. Covert char to priority value */
		c -= (c >= 'a') ? 'a' - 1 : 'A' - 27;
		answer1 += c;

		/* Second part logic here */
		strcpy(lines[idx++], line);
		if (idx == 3) {
			char c = find_common_in_three(lines[0], lines[1], lines[2]);

			/* 3. Covert char to priority value */
			c -= (c >= 'a') ? 'a' - 1 : 'A' - 27;
			answer2 += c;
			idx = 0;
		}
	}
	printf("Answer, part 1: %d\n", answer1);
	printf("Answer, part 2: %d\n", answer2);
}
