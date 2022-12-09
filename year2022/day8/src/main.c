#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

size_t get_num_lines(FILE *f)
{
	char c;
	size_t num_lines = 0;
	rewind(f);

	while ((c = getc(f)) != EOF) {
		if (c == '\n') {
			num_lines++;
		}
	}
	rewind(f);
	return num_lines;
}

enum look_direction {
	LOOK_NORTH,
	LOOK_EAST,
	LOOK_SOUTH,
	LOOK_WEST,
};

bool check_if_visible_from_dir(char **map, size_t columns, size_t rows, size_t x, size_t y,
			       enum look_direction dir, int *scenic_score)
{
	int start;
	int end;
	int positive_dir;
	bool x_axis;

	switch (dir) {
	case LOOK_NORTH: {
		start = y - 1;
		positive_dir = false;
		x_axis = false;
	} break;
	case LOOK_SOUTH: {
		start = y + 1;
		end = rows;
		positive_dir = true;
		x_axis = false;
	} break;
	case LOOK_EAST: {
		start = x + 1;
		end = columns;
		positive_dir = true;
		x_axis = true;

	} break;
	case LOOK_WEST: {
		start = x - 1;
		positive_dir = false;
		x_axis = true;
	} break;
	}

	bool tallest = true;

	struct {
		bool captured;
		int score;
	} scenic;
	scenic.score = 0;
	scenic.captured = false;

	if (positive_dir) {
		for (int k = start; k < end; k++) {
			if (!scenic.captured) {
				scenic.score++;
			}
			if (x_axis) {
				if (map[y][x] <= map[y][k]) {
					tallest = false;
					if (!scenic.captured) {
						scenic.captured = true;
					}
				}
			} else {
				if (map[y][x] <= map[k][x]) {
					tallest = false;
					if (!scenic.captured) {
						scenic.captured = true;
					}
				}
			}
		}
	} else {
		for (int k = start; k >= 0; k--) {
			if (!scenic.captured) {
				scenic.score++;
			}
			if (x_axis) {
				if (map[y][x] <= map[y][k]) {
					tallest = false;
					if (!scenic.captured) {
						scenic.captured = true;
					}
				}
			} else {
				if (map[y][x] <= map[k][x]) {
					tallest = false;
					if (!scenic.captured) {
						scenic.captured = true;
					}
				}
			}
		}
	}
	*scenic_score = scenic.score;

	return tallest;
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
	size_t char_read = 0;
	char **map = malloc(sizeof(char *) * get_num_lines(f));

	size_t idx = 0;

	while (getline(&line, &char_read, f) > 0) {
		map[idx++] = line;
		/* By doing this getline mallocs the memory for line every iteration */
		line = NULL;
	}

	size_t num_columns = strlen(map[0]) - 1;
	size_t answer1 = 0;

	int max_scenic_score = 0;
	int max_i = 0;
	int max_ = 0;
	for (int i = 0; i < idx; i++) {
		for (int j = 0; j < num_columns; j++) {
			/* Check if we are on edge */
			if (i == 0 || i == (idx - 1) || j == 0 || j == (num_columns - 1)) {
				answer1++;
				continue;
			}

			int scenic_scores[4];

			bool visible1 = check_if_visible_from_dir(map, num_columns, idx, j, i,
								  LOOK_NORTH, &scenic_scores[0]);
			bool visible2 = check_if_visible_from_dir(map, num_columns, idx, j, i,
								  LOOK_SOUTH, &scenic_scores[1]);
			bool visible3 = check_if_visible_from_dir(map, num_columns, idx, j, i,
								  LOOK_EAST, &scenic_scores[2]);
			bool visible4 = check_if_visible_from_dir(map, num_columns, idx, j, i,
								  LOOK_WEST, &scenic_scores[3]);

			if (visible1 || visible2 || visible3 || visible4) {
				answer1++;
			}

			int scenic_score = scenic_scores[0] * scenic_scores[1] * scenic_scores[2] *
					   scenic_scores[3];

			if (max_scenic_score < scenic_score) {
				max_scenic_score = scenic_score;
			}
		}
	}

	printf("\n");
	printf("Answer, part 1: %lu\n", answer1);
	printf("Answer, part 2: %i\n", max_scenic_score);
}
