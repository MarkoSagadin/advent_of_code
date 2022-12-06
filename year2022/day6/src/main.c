#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int find_first_n_unique(char *line, size_t n)
{
	char buf[32] = {0};

	for (size_t i = 0; i < strlen(line); i++) {
		buf[(i - 1) % n] = line[i];

		if (i >= n) {
			bool unique = true;
			for (size_t j = 0; j < n; j++) {
				for (size_t k = j + 1; k < n; k++) {
					if (buf[j] == buf[k])
						unique = false;
				}
			}
			if (unique)
				return i + 1;
		}
	}
	return 0;
}

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	FILE *f = fopen("src/input.txt", "r");
	if (!f) {
		printf("No file found!\n");
		return 0;
	}

	char *line = NULL;
	int c;

	size_t char_read = 0;
	getline(&line, &char_read, f);

	printf("Answer, part 1: %i\n", find_first_n_unique(line, 4));
	printf("Answer, part 2: %i\n", find_first_n_unique(line, 14));
}
