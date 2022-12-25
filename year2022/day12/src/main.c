#include <assert.h>
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

enum direction {
	UP = 0,
	DOWN,
	RIGHT,
	LEFT,
	NUM_DIRECTIONS,
	NONE,
};

struct pos {
	int x;
	int y;
};

struct place {
	bool visited;
	bool in_frontier;
	/* Minimal number of steps needed to get to this place from start. */
	int steps;
};

struct pos_place {
	struct pos pos;
	struct place *place;
	/* Priority, 0 is the highest. */
	int prio;
};

struct grid {
	char **map;
	struct place **places;
	int y_size;
	int x_size;
	struct pos start;
	struct pos end;
};

void grid_clean(struct grid *g)
{
	for (int i = 0; i < g->y_size; i++) {
		for (int j = 0; j < g->x_size; j++) {
			g->places[i][j].steps = 10000;
			g->places[i][j].in_frontier = false;
			g->places[i][j].visited = false;
		}
	}
}

#define POS_QUEUE_SIZE 1024
struct pos_place_queue {
	struct pos_place queue[POS_QUEUE_SIZE];
	size_t num;
};

void pos_prio_queue_put(struct pos_place_queue *queue, struct pos_place *pos)
{
	queue->queue[queue->num++] = *pos;
	assert(queue->num < POS_QUEUE_SIZE);
}

void pos_prio_queue_get(struct pos_place_queue *queue, struct pos_place *pos)
{
	assert(queue->num > 0);
	*pos = queue->queue[--queue->num];
}

/* Sort by distance in descending order. */
int compare_pos_places(const void *a, const void *b)
{
	const struct pos_place *pos_a = a;
	const struct pos_place *pos_b = b;

	if (pos_a->prio == pos_b->prio)
		return 0;
	else if (pos_a->prio < pos_b->prio)
		return 1;
	else
		return -1;
}

void pos_prio_queue_sort(struct pos_place_queue *queue)
{
	/* Sort so that the last item in the queue is considered the "best" in priority */
	qsort(queue->queue, queue->num, sizeof(struct pos_place), compare_pos_places);
}

#define RESET	    "[0m" /* Reset to default colors */
#define TEXT_RED    "[2;31m"
#define TEXT_YELLOW "[2;33m"
#define TEXT_BLUE   "[2;34m"

/* Print a grid around current position, inside given range*/
void print_grid(const struct grid *g, const struct pos *current, int range)
{
	int start_x = current->x - range;
	int end_x = current->x + range;
	int start_y = current->y - range;
	int end_y = current->y + range;

	/* Limit the printable range */
	if (start_x < 0)
		start_x = 0;

	if (end_x >= g->x_size)
		end_x = g->x_size - 1;

	if (start_y < 0)
		start_y = 0;
	if (end_y >= g->y_size)
		end_y = g->y_size - 1;

	printf("\n");
	for (int i = start_y; i <= end_y; i++) {
		for (int j = start_x; j <= end_x; j++) {

			if (i == current->y && j == current->x) {
				printf("%s", TEXT_RED);
			} else if (g->places[i][j].in_frontier) {
				printf("%s", TEXT_BLUE);
			} else if (g->places[i][j].visited) {
				printf("%s", TEXT_YELLOW);
			}

			if (i == g->start.y && j == g->start.x) {
				printf("S");
			} else if (i == g->end.y && j == g->end.x) {
				printf("E");
			} else {
				printf("%c", g->map[i][j]);
			}

			if ((i == current->y && j == current->x) || g->places[i][j].visited ||
			    g->places[i][j].in_frontier) {
				printf("%s", RESET);
			}
		}
		printf("\n");
	}
}

/* Manhattan distance on a square grid */
int calc_dist(const struct pos *a, const struct pos *b)
{
	return abs(b->x - a->x) + abs(b->y - a->y);
}

struct pos get_adjecent(const struct grid *g, const struct pos *current, enum direction dir)
{
	struct pos adj = *current;

	switch (dir) {
	case UP: {
		if (current->y > 0)
			adj.y = current->y - 1;
	} break;
	case DOWN: {
		if (current->y < g->y_size - 1)
			adj.y = current->y + 1;
	} break;
	case RIGHT: {
		if (current->x < g->x_size - 1)
			adj.x = current->x + 1;
	} break;
	case LEFT: {
		if (current->x > 0)
			adj.x = current->x - 1;
	} break;
	}

	return adj;
}

bool is_walkable(const struct grid *g, const struct pos *current, const struct pos *next)
{
	char heights[2];

	heights[0] = g->map[current->y][current->x];
	heights[1] = g->map[next->y][next->x];

	for (int i = 0; i < 2; i++) {
		if (heights[i] == 'S')
			heights[i] = 'a';
		if (heights[i] == 'E')
			heights[i] = 'z';
	}

	return (heights[0] + 1 >= heights[1]);
}

int get_valid_adjecents(const struct grid *g, const struct pos *current, struct pos *adjecents)
{
	/* Filter the adjecents positions, positions that are walkable, and were not visited
	 * before.
	 */
	int adjs_nums = 0;
	for (enum direction dir = 0; dir < NUM_DIRECTIONS; dir++) {
		struct pos adj;

		adj = get_adjecent(g, current, dir);

		if (is_walkable(g, current, &adj))
			adjecents[adjs_nums++] = adj;
	}

	return adjs_nums;
}

#include <time.h>

void delay(int ms)
{
	clock_t start_time = clock();
	while (clock() < start_time + ms)
		;
}

bool find_end(const struct grid *g, struct pos_place *start, int *num_steps)
{
	/* Create clean frontier queue */
	struct pos_place_queue frontier;
	memset(&frontier, 0, sizeof(frontier));

	pos_prio_queue_put(&frontier, start);

	/* Loop as long as there is something in frontier */
	while (frontier.num > 0) {
		/* Always sort, before getting */
		pos_prio_queue_sort(&frontier);

		/* Get current position */
		struct pos_place current;
		pos_prio_queue_get(&frontier, &current);
		current.place->in_frontier = false;
		current.place->visited = true;

		// /* Print grid */
		// printf("\e[1;1H\e[2J"); /* Clears screen */
		// print_grid(g, &current.pos, 1000);
		// delay(4000);

		/* Exit early if we reached end */
		if (memcmp(&current.pos, &g->end, sizeof(struct pos)) == 0) {
			*num_steps = current.place->steps;
			return true;
		}

		struct pos adjecents[NUM_DIRECTIONS];

		int num_adjecents = get_valid_adjecents(g, &current.pos, adjecents);

		for (int i = 0; i < num_adjecents; i++) {

			struct pos *next = &adjecents[i];

			int next_step = current.place->steps + 1;

			/* If next was not already visited, or if next has fewer steps (smaller
			 * cost) compared to the one in the grid then add it to frontier and
			 * calculate priority. */
			if (!g->places[next->y][next->x].visited &&
			    next_step < g->places[next->y][next->x].steps) {

				struct pos_place next_place = {
					.place = &g->places[next->y][next->x],
					.prio = next_step + calc_dist(next, &g->end),
					.pos = *next,
				};
				next_place.place->in_frontier = true;
				/* update number steps needed to reach a place */
				next_place.place->steps = next_step;
				pos_prio_queue_put(&frontier, &next_place);
			}
		}
	}
	return false;
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

	/* Create grid object, fill it with the values and find positions for Start and End
	 */
	struct grid grid = {
		.map = malloc(sizeof(char *) * get_num_lines(f)),
	};

	while (getline(&line, &char_read, f) > 0) {
		grid.map[grid.y_size++] = line;
		/* By doing this getline mallocs the memory for line every iteration */
		line = NULL;
	}
	grid.x_size = strlen(grid.map[0]) - 1;
	for (int i = 0; i < grid.y_size; i++) {
		for (int j = 0; j < grid.x_size; j++) {
			if (grid.map[i][j] == 'S') {
				grid.start.x = j;
				grid.start.y = i;
			}
			if (grid.map[i][j] == 'E') {
				grid.end.x = j;
				grid.end.y = i;
			}
		}
	}

	grid.places = malloc(grid.y_size * sizeof(struct place));
	for (int i = 0; i < grid.y_size; i++) {
		grid.places[i] = malloc(grid.x_size * sizeof(struct place));
	}
	grid_clean(&grid);

	/* Add start to the frontier */
	struct pos_place start;
	start.place = &grid.places[grid.start.y][grid.start.y];
	start.pos = grid.start;
	start.prio = 0;
	start.place->steps = 0;

	int num_steps = 0;
	find_end(&grid, &start, &num_steps);
	printf("Answer 1: %i\n", num_steps);

	/* For each 'a' in the map run search, get back the number of steps, keep a track of
	 * minimum. */
	int min_steps = 10000;
	for (int i = 0; i < grid.y_size; i++) {
		for (int j = 0; j < grid.x_size; j++) {
			if (grid.map[i][j] == 'a') {

				start.place = &grid.places[i][j];
				start.pos.y = i;
				start.pos.x = j;
				start.prio = 0;
				start.place->steps = 0;
				if (find_end(&grid, &start, &num_steps)) {
					if (num_steps < min_steps)
						min_steps = num_steps;
				}
				grid_clean(&grid);
			}
		}
	}

	printf("Answer 2: %i\n", min_steps);
}
