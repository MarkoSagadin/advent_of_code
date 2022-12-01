#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *f;

int main()
{
	/* Open file */
	f = fopen("input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	/* Count number of lines in a file */
	int num_lines = get_num_lines(f);
	printf("Num lines is %d\n", num_lines);

	/* Count number of chars in first line */
	int num_chars = get_num_chars_in_line(f);
	printf("Num chars is %d\n\n", num_chars);

	/* Create big enough matrix*/
	char *forest = (char *)malloc(num_lines * num_chars * sizeof(char));

	/* Copy entire content of the file into array */
	copy_file_into_array(f, forest);

	/* Check start of the content */
	printf("First 30 lines of the forest\n");
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < num_chars; j++) {
			printf("%c", *(forest + i * num_chars + j));
		}
		printf("\n");
	}
	printf("\n");

	/* Start moving through the forest */
	struct position {
		int x;
		int y;
	};

	struct position pos = {
		.x = 0,
		.y = 0,
	};

	/* Construct struct of required slopes that we need to search */
	struct step_value {
		int step_right;
		int step_down;
	};

	struct slopes {
		struct step_value step_values[5];
		int tree_counts[5];
	};

	struct slopes slopes1 = {
		.step_values =
			{
				{
					.step_right = 1,
					.step_down = 1,
				},
				{
					.step_right = 3,
					.step_down = 1,
				},
				{
					.step_right = 5,
					.step_down = 1,
				},
				{
					.step_right = 7,
					.step_down = 1,
				},
				{
					.step_right = 1,
					.step_down = 2,
				},
			},
		.tree_counts = {0, 0, 0, 0, 0},
	};

	for (int i = 0; i < 5; i++) {
		while (pos.y != num_lines) {
			/* move */
			pos.x += slopes1.step_values[i].step_right;
			pos.y += slopes1.step_values[i].step_down;

			/* adjust position if you go over the end */
			if (pos.x >= num_chars) {
				pos.x -= num_chars;
			}

			/* Escape early if you are out of forest */
			if (pos.y > num_lines)
				break;

			/* check for tree */
			if ('#' == *(forest + pos.y * num_chars + pos.x))
				slopes1.tree_counts[i]++;
		}

		// reset position for new slope
		pos.x = 0;
		pos.y = 0;
	}

	for (int i = 0; i < 5; i++) {
		printf("In slope %d, we hit %d\n", i, slopes1.tree_counts[i]);
	}
	long long int result = 1;

	for (int i = 0; i < 5; i++) {
		result *= slopes1.tree_counts[i];
	}

	printf("Result is %lld", result);

	fclose(f);
	return 0;
}
