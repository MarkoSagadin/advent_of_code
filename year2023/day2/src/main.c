#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *f;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define MIN(a, b)     (a < b ? a : b)

char *colors[] = {"red", "green", "blue"};

struct cubes {
	int num_rgb_cubes[3];
};

int get_game_id(char *line)
{
	char *tmp = strtok(line, ":");
	tmp = strtok(tmp, " ");
	tmp = strtok(NULL, " ");
	return atoi(tmp);
}

typedef void (*seq_cb_t)(char *item, size_t idx, void *ctx);

/* Split given seq by delim, give each item you split to the callback cb.
 * Pass along ctx and current index of the item in the seq */
void for_each_item_in(char *seq, char *delim, seq_cb_t cb, void *ctx)
{
	size_t idx = 0;
	char *saveprt;
	char *tmp = strtok_r(seq, delim, &saveprt);
	if (!tmp) {
		return;
	}

	cb(tmp, idx++, ctx);

	while (1) {
		tmp = strtok_r(NULL, delim, &saveprt);
		if (!tmp) {
			break;
		}
		cb(tmp, idx++, ctx);
	}
}

void print_item_cb(char *item, void *ctx)
{
	printf("item: %s\n", item);
}

void process_cube_cb(char *cube, size_t idx, void *ctx)
{
	// printf("--->cube[%lu]: %s\n", idx, cube);
	char *saveprt;
	int num = atoi(strtok_r(cube, " ", &saveprt));
	char *color = strtok_r(NULL, " ", &saveprt);

	struct cubes *cubes = ctx;

	for (int i = 0; i < ARRAY_SIZE(colors); i++) {
		if (strcmp(colors[i], color)) {
			continue;
		}
		/* Found match, assign to external variable,
		 * only if the match is larger. */
		if (num > cubes->num_rgb_cubes[i]) {
			cubes->num_rgb_cubes[i] = num;
		}
	}
}

void process_set_cb(char *set, size_t idx, void *ctx)
{
	// printf("->set[%lu]: %s\n", idx, set);
	for_each_item_in(set, ",", process_cube_cb, ctx);
}

// Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
struct cubes get_max_num_cubes_in_game(char *line)
{
	char *tmp = strtok(line, ":");
	tmp = strtok(NULL, ":");

	struct cubes cubes = {0, 0, 0};
	for_each_item_in(tmp, ";", process_set_cb, &cubes);

	return cubes;
}

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	size_t num_char = 1024;
	char *line = malloc(num_char);
	char *tmp_line = malloc(num_char);

	long sum1 = 0;
	long sum2 = 0;

	while (1) {
		if (getline(&line, &num_char, f) == -1)
			break;

		/* Make a temporary copy, as strtok modifies the original string. */
		strcpy(tmp_line, line);

		line[strlen(line) - 1] = '\0';
		printf("\n%s\n", line);
		struct cubes cubes = get_max_num_cubes_in_game(line);

		printf("rgb cubes: %u %u %u\n", cubes.num_rgb_cubes[0], cubes.num_rgb_cubes[1],
		       cubes.num_rgb_cubes[2]);
		if (cubes.num_rgb_cubes[0] <= 12 && cubes.num_rgb_cubes[1] <= 13 &&
		    cubes.num_rgb_cubes[2] <= 14) {
			long id = get_game_id(line);
			sum1 += id;
			printf("ok: id = %lu, sum1 = %lu\n", id, sum1);
		} else {
			printf("not ok\n");
		}
		sum2 += cubes.num_rgb_cubes[0] * cubes.num_rgb_cubes[1] * cubes.num_rgb_cubes[2];
	}

	printf("Sum1 is: %lu\n", sum1);
	printf("Sum2 is: %lu\n", sum2);
}
