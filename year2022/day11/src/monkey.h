/** @file monkey.h
 *
 * @brief
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Irnas. All rights reserved.
 * Author: Marko Sagadin <marko@irnas.eu>
 */

#ifndef MONKEY_H
#define MONKEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

struct monkey {
	uint64_t items[100];
	size_t num_items;
	char operation;
	char *operant;
	uint64_t test_div;
	/* Who to throw if test is true */
	int true_throw_idx;
	/* Who to throw if test is false */
	int false_throw_idx;
	/* Incremented everytime when an item is inspected */
	int inspect_count;
};

void prepare_monkeys(struct monkey **m, int *num_monkeys);
void prepare_test_monkeys(struct monkey **m, int *num_monkeys);

#ifdef __cplusplus
}
#endif

#endif /* MONKEY_H */
