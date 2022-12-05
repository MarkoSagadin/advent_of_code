#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	FILE *f = fopen("src/input.txt", "r");
	if (!f) {
		printf("No file found!\n");
		return 0;
	}

	/* Steps for first part
	 * 1. split string into two ranges, a and b
	 * 2. Determine if range a fits into range b, and vice versa
	 * */
	size_t num_char = 128;
	char *line = malloc(num_char);
	int answer1 = 0;
	int answer2 = 0;
	int a[2];
	int b[2];
	bool also_contained = false;

	while (getline(&line, &num_char, f) != -1) {

		a[0] = atoi(strtok(line, "-,"));
		a[1] = atoi(strtok(NULL, "-,"));
		b[0] = atoi(strtok(NULL, "-,"));
		b[1] = atoi(strtok(NULL, "-,"));

		if ((b[0] <= a[0] && a[1] <= b[1]) || (a[0] <= b[0] && b[1] <= a[1])) {
			answer1++;
			also_contained = true;
		}

		if ((b[0] <= a[0] && a[0] <= b[1]) || (b[0] <= a[1] && a[1] <= b[1]) ||
		    also_contained) {
			also_contained = false;
			answer2++;
		}
	}

	printf("Answer, part 1: %d\n", answer1);
	printf("Answer, part 2: %d\n", answer2);
}
