#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct rock_point {
	int x;
	int y;
};

/* Some cool struct */
struct cave_point {
	bool rock;
	bool sand;
};

struct cave {
	struct cave_point **grid;
	size_t x_size;
	size_t y_size;
};

#define CAVE_WIDTH	     4000
#define CAVE_POSITION_OFFSET (CAVE_WIDTH / 2 - 500)
#define SOURCE_POSITION	     (500 + CAVE_POSITION_OFFSET)

void cave_init(struct cave *cave)
{
	/* Start with width of 4000, and depth of 0 */
	cave->x_size = CAVE_WIDTH;
	cave->y_size = 0;
}

void cave_extend_down(struct cave *cave, size_t len)
{
	cave->grid = reallocarray(cave->grid, cave->y_size + len, sizeof(struct cave_point *));

	for (size_t i = cave->y_size; i < cave->y_size + len; i++) {
		cave->grid[i] = calloc(cave->x_size, sizeof(struct cave_point));
	}

	cave->y_size += len;
}

/* Use this function, when you want to operate on the cave point, due to the offset */
struct cave_point *cave_get_point(struct cave *cave, size_t y, size_t x)
{
	return &cave->grid[y][x + CAVE_POSITION_OFFSET];
}

struct rock_point *parse_line_into_rock_points(char *line, size_t *num_rock_points)
{
	char *delim = "->";
	char *token = line;
	*num_rock_points = 0;
	struct rock_point *rock_points = NULL;

	while ((token = strtok(token, delim))) {
		struct rock_point point;
		sscanf(token, "%i,%i", &point.x, &point.y);
		rock_points =
			reallocarray(rock_points, ++(*num_rock_points), sizeof(struct rock_point));
		rock_points[*num_rock_points - 1] = point;

		/* Set token to NULL to keep moving with strtok. */
		token = NULL;
	}

	return rock_points;
}

void cave_add_rock_point(struct cave *cave, struct rock_point *point)
{
	/* Check if the new point is in the depth range of the cave, extend it if not. */
	if (cave->y_size < point->y + 1) {
		cave_extend_down(cave, point->y + 1 - cave->y_size);
	}
	struct cave_point *p = cave_get_point(cave, point->y, point->x);
	p->rock = true;
}

void cave_add_rock_points(struct cave *cave, struct rock_point *rock_points, size_t num_rock_points)
{
	for (size_t i = 0; i < num_rock_points - 1; i++) {
		struct rock_point point;

		/* Add points in horizontal direction */
		if (rock_points[i].x != rock_points[i + 1].x) {
			int dir = rock_points[i].x < rock_points[i + 1].x ? 1 : -1;

			int x = rock_points[i].x;
			while (1) {
				point.x = x;
				point.y = rock_points[i].y;
				cave_add_rock_point(cave, &point);

				if (x == rock_points[i + 1].x) {
					break;
				}
				x = x + dir;
			}
		}

		/* Add points in vertical direction */
		if (rock_points[i].y != rock_points[i + 1].y) {
			int dir = rock_points[i].y < rock_points[i + 1].y ? 1 : -1;
			int y = rock_points[i].y;
			while (1) {
				point.x = rock_points[i].x;
				point.y = y;
				cave_add_rock_point(cave, &point);

				if (y == rock_points[i + 1].y) {
					break;
				}
				y = y + dir;
			}
		}
	}
}

void print_cave(struct cave *cave, size_t range)
{
	int start_x = SOURCE_POSITION - range;
	int end_x = SOURCE_POSITION + range;

	printf("\n");
	for (int i = 0; i < cave->y_size; i++) {
		for (int j = start_x; j < end_x; j++) {

			if (i == 0 && j == SOURCE_POSITION) {
				printf("+");
			} else if (cave->grid[i][j].rock) {
				printf("#");
			} else if (cave->grid[i][j].sand) {
				printf("o");
			} else {
				printf(".");
			}
		}
		printf("\n");
	}
}

bool is_point_occupied(struct cave *cave, int y, int x)
{
	return (cave_get_point(cave, y, x)->rock || cave_get_point(cave, y, x)->sand);
}

/* Return true if sand came to rest, return false if it fell into void.*/
bool simulate_falling_sand_lump(struct cave *cave)
{
	int sand_x = 500;
	int sand_y = 0;

	while (1) {
		/* Check if it is almost at the end of the cave range */
		if (cave->y_size == sand_y + 1) {
			return false;
		}

		/* Check if it can move down */
		if (!is_point_occupied(cave, sand_y + 1, sand_x)) {
			sand_y++;
			continue;
		}

		/* Check if it can move down-left */
		if (!is_point_occupied(cave, sand_y + 1, sand_x - 1)) {
			sand_y++;
			sand_x--;
			continue;
		}

		/* Check if it can move down-right */
		if (!is_point_occupied(cave, sand_y + 1, sand_x + 1)) {
			sand_y++;
			sand_x++;
			continue;
		}

		/* Can't move, mark position. return true */
		cave_get_point(cave, sand_y, sand_x)->sand = true;
		return true;
	}
}

// 498,4 -> 498,6 -> 496,6
// 503,4 -> 502,4 -> 502,9 -> 494,9
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
	struct element *element = NULL;

	struct cave cave;
	cave_init(&cave);

	size_t num_rock_points;
	struct rock_point *rock_points;

	/* Get line */
	while (getline(&line, &char_read, f) > 0) {
		/* parse line into rock points */
		rock_points = parse_line_into_rock_points(line, &num_rock_points);
		/* apply rock points to the cave grid */
		cave_add_rock_points(&cave, rock_points, num_rock_points);

		/* Set line to NULL so getline can get malloc new line */
		line = NULL;
	}
	// print_cave(&cave, 20);

	int num_sand_lumps = 0;
	while (1) {
		if (!simulate_falling_sand_lump(&cave)) {
			break;
		}
		num_sand_lumps++;
	}
	printf("Answer, part 1: %i\n", num_sand_lumps);

	/* Make some space */
	cave_extend_down(&cave, 2);

	/* Create rock floor */
	for (size_t i = 0; i < cave.x_size; i++) {
		cave.grid[cave.y_size - 1][i].rock = true;
	}

	/* Keep filling with sand until source is blocked with it */
	while (1) {
		simulate_falling_sand_lump(&cave);
		num_sand_lumps++;
		if (cave_get_point(&cave, 0, 500)->sand) {
			break;
		}
	}

	// print_cave(&cave, 40);
	printf("Answer, part 2: %i\n", num_sand_lumps);
}
