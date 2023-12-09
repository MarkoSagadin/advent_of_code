#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	void name##_insert(struct name *vec, type *item)                                           \
	{                                                                                          \
		if (vec->num == vec->size) {                                                       \
			vec->size *= 2;                                                            \
			vec->items = realloc(vec->items, vec->size * sizeof(type));                \
		}                                                                                  \
		vec->items[vec->num++] = *item;                                                    \
	}

DEFINE_VEC(history, int);
DEFINE_VEC(histories, struct history);

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

void print_history(struct history *hist)
{
	for (size_t i = 0; i < hist->num; i++) {
		printf("%i ", hist->items[i]);
	}
}

void print_histories(struct histories *hists)
{
	for (size_t i = 0; i < hists->num; i++) {
		printf("hist[%lu], num=%lu: ", i, hists->items[i].num);
		print_history(&hists->items[i]);
		printf("\n");
	}
}

void history_parse_from_line(char *line, struct history *hist)
{
	char *next;
	while (1) {

		int val = strtol(line, &next, 10);
		history_insert(hist, &val);

		if (*next++ == '\n') {
			return;
		}
		line = next;
	}
}

void histories_parse_from_file(FILE *f, struct histories *hists)
{
	while (1) {
		char *line = malloc(256);
		size_t num_char;
		if (getline(&line, &num_char, f) == -1) {
			break;
		}
		struct history hist;
		history_init(&hist, 10);

		history_parse_from_line(line, &hist);
		histories_insert(hists, &hist);
	}
}

void history_calc_diffs(struct history *hist, struct history *diffs)
{
	history_init(diffs, 10);

	for (size_t i = 0; i < hist->num - 1; i++) {
		int d = hist->items[i + 1] - hist->items[i];
		history_insert(diffs, &d);
	}
}

bool history_all_zero(struct history *hist)
{
	for (size_t i = 0; i < hist->num; i++) {
		if (hist->items[i] != 0) {
			return false;
		}
	}
	return true;
}

int history_predict(struct history *hist, bool front)
{
	/* Check if diffs are all zero */
	if (history_all_zero(hist)) {
		/* If yes, return 0 */
		return 0;
	}

	struct history diffs;

	/* From hist calc diffs */
	history_calc_diffs(hist, &diffs);

	/* If not call history_predict with diffs. */
	int val = history_predict(&diffs, front);

	/* Which will return the next diff */

	/* Use the predicted value to calc the next element in given hist and return it. */
	return front ? hist->items[hist->num - 1] + val : hist->items[0] - val;
}

int main()
{
	FILE *f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	struct histories hists;
	histories_init(&hists, 10);

	histories_parse_from_file(f, &hists);

	// print_histories(&hists);

	int sum1 = 0;
	int sum2 = 0;

	for (size_t i = 0; i < hists.num; i++) {
		sum1 += history_predict(&hists.items[i], true);
		sum2 += history_predict(&hists.items[i], false);
	}

	printf("Part 1: %i\n", sum1);
	printf("Part 2: %i\n", sum2);
}
