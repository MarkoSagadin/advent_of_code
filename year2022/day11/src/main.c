#include "monkey.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int test_item(struct monkey *m, int item_idx, bool divide_by_3)
{
	uint64_t *item = &m->items[item_idx];

	uint64_t operant;

	if (strcmp("old", m->operant) == 0) {
		operant = *item;
	} else {
		operant = atoi(m->operant);
	}

	switch (m->operation) {
	case '+': {
		*item += operant;
	} break;
	case '*': {
		*item *= operant;
	} break;
	}

	if (divide_by_3) {
		*item /= 3;
	} else {
		uint64_t solution = 9699690;
		*item = *item % solution;
	}

	if ((*item % m->test_div) == 0) {
		return m->true_throw_idx;
	} else {
		return m->false_throw_idx;
	}
}

uint64_t monkeys_start_throwing(struct monkey *monkeys, int num_monkeys, bool part1)
{
	int num_rounds = part1 ? 20 : 10000;
	int divide_by_3 = part1 ? true : false;

	for (size_t i = 0; i < num_rounds; i++) {
		// printf("**Round %zu**\n", i);
		for (size_t j = 0; j < num_monkeys; j++) {
			for (size_t k = 0; k < monkeys[j].num_items; k++) {
				int monkex_idx = test_item(&monkeys[j], k, divide_by_3);

				monkeys[j].inspect_count++;
				struct monkey *m = &monkeys[monkex_idx];
				m->items[m->num_items++] = monkeys[j].items[k];
			}
			monkeys[j].num_items = 0;
		}
	}

	/* Find maximum two inspect counts and multiply them */
	uint64_t inspect_counts[2] = {0};
	for (size_t j = 0; j < num_monkeys; j++) {
		if (inspect_counts[1] < monkeys[j].inspect_count) {
			inspect_counts[1] = monkeys[j].inspect_count;

			if (inspect_counts[0] < inspect_counts[1]) {
				int tmp = inspect_counts[0];
				inspect_counts[0] = inspect_counts[1];
				inspect_counts[1] = tmp;
			}
		}
	}
	return inspect_counts[0] * inspect_counts[1];
}

int main()
{
	int num_monkeys;
	struct monkey *monkeys;

	// prepare_test_monkeys(&monkeys, &num_monkeys);

	prepare_monkeys(&monkeys, &num_monkeys);

	int part1 = true;
	int answer1 = monkeys_start_throwing(monkeys, num_monkeys, part1);

	prepare_monkeys(&monkeys, &num_monkeys);
	part1 = false;
	uint64_t answer2 = monkeys_start_throwing(monkeys, num_monkeys, part1);

	printf("Answer, part 1: %i \n", answer1);
	printf("Answer, part 2: %lu\n", answer2);
}
