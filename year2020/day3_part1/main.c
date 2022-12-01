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

	int step_right = 3;
	int step_down = 1;
	int tree_count = 0;

	while (pos.y != num_lines) {
		/* move */
		pos.x += step_right;
		pos.y += step_down;

		/* adjust position if you go over the end */
		if (pos.x >= num_chars) {
			pos.x -= num_chars;
		}

		/* check for tree */
		if ('#' == *(forest + pos.y * num_chars + pos.x))
			tree_count++;
	}

	printf("We hit %d trees!", tree_count);
	fclose(f);
	return 0;
}
