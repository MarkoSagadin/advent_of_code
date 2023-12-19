#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <time.h>

void delay(int ms)
{
	clock_t start_time = clock();
	while (clock() < start_time + ms)
		;
}

#define DEFINE_VEC(name, type)                                                                     \
	struct name {                                                                              \
		type *items;                                                                       \
		size_t num;                                                                        \
		size_t size;                                                                       \
	};                                                                                         \
                                                                                                   \
	void name##_init(struct name *vec, size_t init_size)                                       \
	{                                                                                          \
		vec->items = malloc(init_size * sizeof(type));                                     \
		vec->num = 0;                                                                      \
		vec->size = init_size;                                                             \
	}                                                                                          \
                                                                                                   \
	void name##_put(struct name *vec, type *item)                                              \
	{                                                                                          \
		if (vec->num == vec->size) {                                                       \
			vec->size *= 2;                                                            \
			vec->items = realloc(vec->items, vec->size * sizeof(type));                \
		}                                                                                  \
		vec->items[vec->num++] = *item;                                                    \
	}                                                                                          \
	void name##_get(struct name *vec, type *item)                                              \
	{                                                                                          \
		*item = vec->items[--vec->num];                                                    \
	}

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

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

DEFINE_VEC(pos_queue, struct pos);

struct grid {
	struct grid_cell **grid;
	size_t num_rows;
	size_t num_cols;
};

struct grid_cell {
	char tile;
	bool is_pipe;
	bool is_outside;
	bool processed;
};

inline char grid_get_tile(struct grid *g, struct pos *p)
{
	return g->grid[p->y][p->x].tile;
}

inline struct grid_cell *grid_get_cell(struct grid *g, struct pos *p)
{
	return &g->grid[p->y][p->x];
}

struct grid_cell *grid_cells_create(char *line)
{
	struct grid_cell *cells = NULL;
	size_t size = 0;

	for (size_t i = 0; i < strlen(line) - 1; i++) {

		struct grid_cell cell = {
			.tile = line[i],
		};
		cells = realloc(cells, (size + 1) * sizeof(struct grid_cell));
		cells[size++] = cell;
	}

	return cells;
}

void grid_create(FILE *f, struct grid *g)
{
	g->grid = NULL;
	g->num_cols = 0;

	char *line;

	while (1) {
		line = NULL;
		size_t num_char = 0;
		if (getline(&line, &num_char, f) == -1) {
			break;
		}

		struct grid_cell *grid_cell = grid_cells_create(line);

		g->grid = reallocarray(g->grid, g->num_rows + 1, sizeof(struct grid_cell *));
		if (!g->grid) {
			wprintf(L"reallocarray failed");
			exit(-1);
		}
		g->grid[g->num_rows++] = grid_cell;
		g->num_cols = strlen(line) - 1;
	}
}

#define RESET	    "[0m" /* Reset to default colors */
#define TEXT_RED    "[2;31m"
#define TEXT_YELLOW "[2;33m"
#define TEXT_BLUE   "[2;34m"

wchar_t tile_pretty_pipe(struct grid_cell *c)
{
	switch (c->tile) {
	case '|':
		return L'│';
		break;
	case '-':
		return L'─';
		break;
	case 'L':
		return L'└';
		break;
	case 'J':
		return L'┘';
		break;
	case '7':
		return L'┐';
		break;
	case 'F':
		return L'┌';
		break;
	}

	/* Convert ASCII char to unicode equivalent. */
	char ch[2] = {c->tile, '\0'};
	static wchar_t uni[5];

	mbstowcs(uni, ch, 2);
	return uni[0];
}

inline bool are_pos_same(struct pos *p1, struct pos *p2)
{
	return (p1->x == p2->x && p1->y == p2->y);
}

void grid_print(struct grid *g, struct pos *marker)
{
	for (size_t i = 0; i < g->num_rows; i++) {
		for (size_t j = 0; j < g->num_cols; j++) {
			struct grid_cell *c = grid_get_cell(g, &(struct pos){j, i});

			/* Apply color to tiles with special meaning. */
			if (c->is_pipe) {
				wprintf(L"%s", TEXT_YELLOW);
			}
			if (c->is_outside) {
				wprintf(L"%s", TEXT_BLUE);
			}

			bool marked = marker && are_pos_same(marker, &(struct pos){j, i});
			if (marked) {
				wprintf(L"%s╬", TEXT_RED);
			} else {
				wprintf(L"%lc", tile_pretty_pipe(c));
			}

			/* Reset color of tiles with special meaning. */
			if (c->is_pipe || c->is_outside || marked) {
				wprintf(L"%s", RESET);
			}
		}
		wprintf(L"\n\r");
	}
}

struct pos find_start(struct grid *g)
{
	for (size_t i = 0; i < g->num_rows; i++) {
		for (size_t j = 0; j < g->num_cols; j++) {
			struct pos p = {
				.x = j,
				.y = i,
			};
			if (grid_get_tile(g, &p) == 'S') {
				return p;
			}
		}
	}

	return (struct pos){
		.x = 0,
		.y = 0,
	};
}

struct pos find_pipe(struct grid *g)
{
	for (size_t i = 0; i < g->num_rows; i++) {
		for (size_t j = 0; j < g->num_cols; j++) {
			struct pos p = {
				.x = j,
				.y = i,
			};
			if (grid_get_cell(g, &p)->is_pipe) {
				return p;
			}
		}
	}

	return (struct pos){
		.x = 0,
		.y = 0,
	};
}

/* Parsing function to read file into 2d array
 * Find S function, returns position.
 * Step function. I will call it in while loop, until the S position is reached
 *
 * How to step?
 * Keep a track of current position and previous position. Check the tile on the current position.
 * Each tile will offer to possible positions into which you can move. You should move on the one
 * that is not your previous position.
 * .*/
struct pos get_next_pos_from_start(struct grid *g, struct pos *cur)
{
	struct pos north = {
		.x = cur->x,
		.y = cur->y - 1,
	};
	struct pos south = {
		.x = cur->x,
		.y = cur->y + 1,
	};
	struct pos east = {
		.x = cur->x + 1,
		.y = cur->y,
	};
	struct pos west = {
		.x = cur->x - 1,
		.y = cur->y,
	};

	char n = grid_get_tile(g, &north);
	char s = grid_get_tile(g, &south);
	char e = grid_get_tile(g, &east);
	char w = grid_get_tile(g, &west);

	if (n == '|' || n == '7' || n == 'F') {
		return north;
	}

	if (s == '|' || s == 'L' || s == 'J') {
		return south;
	}

	if (e == '-' || e == '7' || e == 'J') {
		return east;
	}

	// if (w == '-' || w == 'L' || w == 'F') {
	return west;
}

void get_adjecent_pos(struct grid *g, struct pos *cur, struct pos *adj1, struct pos *adj2)
{
	char c = grid_get_tile(g, cur);

	switch (c) {
	case '|':
		adj1->x = cur->x;
		adj1->y = cur->y - 1;
		adj2->x = cur->x;
		adj2->y = cur->y + 1;
		break;
	case '-':
		adj1->x = cur->x - 1;
		adj1->y = cur->y;
		adj2->x = cur->x + 1;
		adj2->y = cur->y;
		break;
	case 'L':
		adj1->x = cur->x;
		adj1->y = cur->y - 1;
		adj2->x = cur->x + 1;
		adj2->y = cur->y;
		break;
	case 'J':
		adj1->x = cur->x;
		adj1->y = cur->y - 1;
		adj2->x = cur->x - 1;
		adj2->y = cur->y;
		break;
	case '7':
		adj1->x = cur->x;
		adj1->y = cur->y + 1;
		adj2->x = cur->x - 1;
		adj2->y = cur->y;
		break;
	case 'F':
		adj1->x = cur->x;
		adj1->y = cur->y + 1;
		adj2->x = cur->x + 1;
		adj2->y = cur->y;
		break;
	}
}

void step_in_pipe(struct grid *g, struct pos *cur, struct pos *prev)
{

	struct pos adj1, adj2;

	grid_get_cell(g, cur)->is_pipe = true;

	/* Handle special case at the beginning */
	if (grid_get_tile(g, cur) == 'S') {
		*prev = *cur;
		*cur = get_next_pos_from_start(g, cur);
		return;
	}

	get_adjecent_pos(g, cur, &adj1, &adj2);
	// printf("adj1: %c, adj2: %c, prev: %c\n", grid_get_tile(g, &adj1), grid_get_tile(g,
	// &adj2),
	//        grid_get_tile(g, &prev));

	struct pos next = are_pos_same(prev, &adj1) ? adj2 : adj1;
	*prev = *cur;
	*cur = next;
}

int first_part(struct grid *g)
{
	struct pos start = find_start(g);
	struct pos *cur = &start;

	int num_steps = 0;

	struct pos prev = {0, 0};
	while (1) {
		step_in_pipe(g, cur, &prev);
		num_steps++;

		if (grid_get_tile(g, cur) == 'S') {
			break;
		}
	}

	return num_steps / 2;
}

void find_all_cells_on_edge(struct grid *g, struct pos_queue *out)
{
	for (size_t i = 0; i < g->num_rows; i++) {
		for (size_t j = 0; j < g->num_cols; j++) {
			struct pos p = {
				.x = j,
				.y = i,
			};
			/* Check if the cell is on the edge and not a pipe, if yes, add it to the
			 * queue. */
			if ((p.x == 0 || p.x == g->num_cols - 1 || p.y == 0 ||
			     p.y == g->num_rows - 1) &&
			    !(grid_get_cell(g, &p)->is_pipe)) {
				grid_get_cell(g, &p)->is_outside = true;
				pos_queue_put(out, &p);
			}
		}
	}
}

int get_adjecent(struct grid *g, const struct pos *p, struct pos *adj, enum direction dir)
{
	*adj = *p;

	switch (dir) {
	case UP:
		if (p->y > 0) {
			adj->y = p->y - 1;
			return 0;
		}
		break;
	case DOWN:
		if (p->y < g->num_rows - 1) {
			adj->y = p->y + 1;
			return 0;
		}
		break;
	case RIGHT:
		if (p->x < g->num_cols - 1) {
			adj->x = p->x + 1;
			return 0;
		}
		break;
	case LEFT:
		if (p->x > 0) {
			adj->x = p->x - 1;
			return 0;
		}
		break;
	default:
		return -1;
	}

	return -1;
}

void get_valid_adjecents(struct grid *g, struct pos *p, struct pos_queue *adjs)
{
	for (enum direction dir = 0; dir < NUM_DIRECTIONS; dir++) {
		struct pos adj;
		int rc = get_adjecent(g, p, &adj, dir);
		if (rc) {
			continue;
		}

		struct grid_cell *c = grid_get_cell(g, &adj);
		if (c->is_pipe) {
			continue;
		}

		if (!c->processed) {
			c->is_outside = true;
			pos_queue_put(adjs, &adj);
		}
	}
}
enum direction get_dir(struct pos *prev, struct pos *cur)
{
	int dx = cur->x - prev->x;
	int dy = cur->y - prev->y;

	/* Moved LEFT or RIGHT */
	if (dy == 0) {
		return dx > 0 ? RIGHT : LEFT;
	}

	/* Moved UP or DOWN */
	return dy > 0 ? DOWN : UP;
}

enum direction get_outside_dir(enum direction dir)
{
	switch (dir) {
	case UP:
		return LEFT;
		break;
	case DOWN:
		return RIGHT;
		break;
	case RIGHT:
		return UP;
		break;
	case LEFT:
		return DOWN;
		break;
	default:
		return DOWN;
	}
}

void mark_as_outside_in_direction(struct grid *g, enum direction out_dir, struct pos *cur)
{
	bool move_vert = (UP == out_dir || DOWN == out_dir);
	int step = (RIGHT == out_dir || DOWN == out_dir) ? 1 : -1;

	struct pos scanner = *cur;
	while (1) {
		if (move_vert) {
			scanner.y += step;
		} else {
			scanner.x += step;
		}

		if (scanner.y < 0 || scanner.y == g->num_rows || scanner.x < 0 ||
		    scanner.x == g->num_cols) {
			break;
		}

		struct grid_cell *c = grid_get_cell(g, &scanner);

		if (c->is_pipe) {
			break;
		}
		c->is_outside = true;
	}
}

/* Solving second part
 * Go around the edge of the map and mark each cell as outside (only if it is not pipe already).
 * Put each outside cell into a queue. Then start popping each cell out of the queue and look at it
 * adjecents.
 *
 * Check that they are not pipe and already not outside, if yes, then,
 * mark them outside and put them into queue.
 *
 * However the above approach will only find all outside cells that are not hiding between the
 * pipes.
 * To get the other ones we can take the advantage of the fact that on side of the pipe is always
 * considered outside and the other one is always considered inside.
 *
 * Start scanning the grid until you hit the first pipe cell. This is your new start point. Start
 * stepping in the pipe. Looking from the direction of movement, left side is outside, right side is
 * inside. On every step look to the left side and mark any nonmarked cells as outside until you hit
 * the pipe cell or the edge of the grid. On pipes that go straight you only need to scan in one
 * direction, in pipes that turn you need to scan in two directions. You can probably make this a
 * constant rule.
 * */

int second_part(struct grid *g)
{
	struct pos_queue out;
	pos_queue_init(&out, 1);
	find_all_cells_on_edge(g, &out);

	int i = 0;
	while (out.num > 0) {
		struct pos p;

		pos_queue_get(&out, &p);
		grid_get_cell(g, &p)->processed = true;

		get_valid_adjecents(g, &p, &out);
	}

	struct pos start = find_start(g);

	struct pos cur = start;

	struct pos prev = {0, 0};

	while (1) {
		step_in_pipe(g, &cur, &prev);
		// grid_print(g, &cur);

		/* First get direction into which we moved, then get direction left to it. */
		enum direction out_dir = get_outside_dir(get_dir(&prev, &cur));

		mark_as_outside_in_direction(g, out_dir, &cur);
		mark_as_outside_in_direction(g, out_dir, &prev);

		if (are_pos_same(&cur, &start)) {
			break;
		}
		// delay(50000);
		// wprintf(L"\e[1;1H\e[2J"); /* Clears screen */
	}

	/* Count all non-pipe, non-outside cells, those are inside. */
	int num_inside_cells = 0;
	for (size_t i = 0; i < g->num_rows; i++) {
		for (size_t j = 0; j < g->num_cols; j++) {
			struct grid_cell *c = grid_get_cell(g, &(struct pos){j, i});
			if (!c->is_pipe && !c->is_outside) {
				num_inside_cells++;
			}
		}
	}

	return num_inside_cells;
}

int main()
{
	FILE *f = fopen("src/input.txt", "r");
	if (f == NULL) {
		wprintf(L"No file found!\n");
		return 0;
	}
	setlocale(LC_CTYPE, "");

	struct grid g;
	grid_create(f, &g);

	// grid_print(&g);
	wprintf(L"%lu %lu\n", g.num_rows, g.num_cols);

	int num_steps = first_part(&g);

	int num_in = second_part(&g);
	grid_print(&g, NULL);

	wprintf(L"Part 1: %i\n", num_steps);
	wprintf(L"Part 2: %i\n", num_in);
}
