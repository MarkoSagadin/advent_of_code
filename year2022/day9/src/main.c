#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct pos {
	int x;
	int y;
};

struct dir_step {
	/* Number of steps needed for the head to move.*/
	int steps;
	/* If true then we are moving along x direction, otherwise along y. */
	bool x_dir;
	/* If true then we are moving in positive direction, otherwise negative */
	bool positive;
};

struct dir_step determine_dir_step(char dir, int step)
{
	struct dir_step dir_step;

	switch (dir) {
	case 'U':
		dir_step.positive = true;
		dir_step.x_dir = false;
		break;
	case 'D':
		dir_step.positive = false;
		dir_step.x_dir = false;
		break;
	case 'R':
		dir_step.positive = true;
		dir_step.x_dir = true;
		break;
	case 'L':
		dir_step.positive = false;
		dir_step.x_dir = true;
		break;
	}
	dir_step.steps = step;

	return dir_step;
}

void move_tail(struct pos *head, struct pos *tail)
{
	const int max_len = 1;

	int x_delta = head->x - tail->x;
	int y_delta = head->y - tail->y;

	if (abs(x_delta) > max_len) {
		if (abs(y_delta) > 0) {
			tail->y += y_delta > 0 ? 1 : -1;
		}
		tail->x += x_delta > 0 ? 1 : -1;
		return;
	}
	if (abs(y_delta) > max_len) {
		if (abs(x_delta) > 0) {
			tail->x += x_delta > 0 ? 1 : -1;
		}
		tail->y += y_delta > 0 ? 1 : -1;
		return;
	}
}

void print_map(struct pos *head, struct pos *tail, bool *map, int num_columns, int num_rows)
{
	const int size = 6;
	for (int i = size - 1; i >= 0; i--) {
		for (int k = 0; k < size; k++) {
			if (i == head->y && k == head->x) {
				printf("H");
			} else if (i == tail->y && k == tail->x) {
				printf("T");
			} else if (i < num_rows && map[i * num_columns + k]) {
				printf("#");
			} else {
				printf(".");
			}
			if (k == size - 1) {
				printf("\n");
			}
		}
	}

	printf("\n");
	// printf("-> head[%i][%i], tail[%i][%i]\n", head->y, head->x, tail->y, tail->x);
}

void perform_steps(struct dir_step *dir_step, struct pos *rope, bool *map1, bool *map2,
		   int num_columns, int num_rows)
{
	/* Determine the delta outside of the loop */
	int delta = dir_step->positive ? 1 : -1;

	/* Run loop for the number of steps */
	while (dir_step->steps--) {

		/* Move head one step, depending on the direction */
		if (dir_step->x_dir) {
			rope[0].x += delta;
		} else {
			rope[0].y += delta;
		}

		for (size_t i = 0; i < 9; i++) {
			move_tail(&rope[i], &rope[i + 1]);
		}
		/* Move tail by the rules */

		/* Update both maps, add the offsets */
		map1[(rope[1].y + 1000) * num_columns + (rope[1].x + 1000)] = true;
		map2[(rope[9].y + 1000) * num_columns + (rope[9].x + 1000)] = true;

		// print_map(head, tail, map, num_columns, num_rows);
	}
}

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	FILE *f = fopen("src/input.txt", "r");
	if (!f) {
		printf("No file found!\n");
		return 0;
	}

	/* Hard code map size, we will assume that we will start at coordinates (1000,1000) when
	 * marking the map with tail location. */
	int x = 2000;
	int y = 2000;

	/* Create two maps for keeping track of visited locations, they are larger than they have to
	 * be, but that is ok. Map should be 2d but is not */
	bool *map1 = malloc(x * y * sizeof(bool));
	bool *map2 = malloc(x * y * sizeof(bool));

	memset(map1, 0, x * y);
	memset(map2, 0, x * y);

	struct pos rope[10] = {0};

	char dir;
	int step;
	while (fscanf(f, "%s %i", &dir, &step) != EOF) {

		// printf("%c %i\n", dir, step);
		/* Determine needed steps and direction */
		struct dir_step dir_step = determine_dir_step(dir, step);

		/* Perform them, while marking the tail's path*/
		perform_steps(&dir_step, rope, map1, map2, x, y);
	}

	int answer1 = 0;
	int answer2 = 0;
	for (int i = 0; i < x * y; i++) {
		if (map1[i]) {
			answer1++;
		}
		if (map2[i]) {
			answer2++;
		}
	}

	printf("Answer, part 1: %i\n", answer1);
	printf("Answer, part 2: %i\n", answer2);
}
