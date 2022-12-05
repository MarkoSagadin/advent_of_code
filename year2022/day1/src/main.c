#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *f;

void swap(long *x, long *y)
{
	long tmp = *x;
	*x = *y;
	*y = tmp;
}

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	size_t num_char = 128;
	char *line = malloc(num_char);
	long sum = 0;
	long max[3] = {0, 0, 0};
	long tmp = 0;

	/* Steps
	 * 1. Get line and convert into number
	 * 2. If it can get converted to number add it to sum.
	 * 3. If it can not get converted to a number check if sum can become max
	 * 4. repeat 1., 2., 3. until the end.
	 * */
	while (1) {
		if (getline(&line, &num_char, f) == -1)
			break;

		/* Just assing something */
		char *endptr = line;

		long num = strtol(line, &endptr, 10);
		if (line == endptr) {
			/* empty line detected */

			if (sum > max[0]) {
				max[0] = sum;
				for (size_t i = 0; i < 2; i++) {
					if (max[i] > max[i + 1]) {
						swap(&max[i], &max[i + 1]);
					}
				}
			}

			sum = 0;
		} else {
			sum += num;
		}

		// printf("num: %lu\n", num);
	}

	printf("Top max sum is: %lu\n", max[2]);
	printf("Sum of top three maxs: %lu\n", max[0] + max[1] + max[2]);
}
