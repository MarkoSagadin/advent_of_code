#include "monkey.h"

static struct monkey monkeys[8];
static struct monkey test_monkeys[4];

void prepare_monkeys(struct monkey **m, int *num_monkeys)
{
	monkeys[0] = (struct monkey){
		.items = {50, 70, 89, 75, 66, 66},
		.num_items = 6,
		.operation = '*',
		.operant = "5",
		.test_div = 2,
		.true_throw_idx = 2,
		.false_throw_idx = 1,
	};

	monkeys[1] = (struct monkey){
		.items = {85},
		.num_items = 1,
		.operation = '*',
		.operant = "old",
		.test_div = 7,
		.true_throw_idx = 3,
		.false_throw_idx = 6,
	};
	monkeys[2] = (struct monkey){
		.items = {66, 51, 71, 76, 58, 55, 58, 60},
		.num_items = 8,
		.operation = '+',
		.operant = "1",
		.test_div = 13,
		.true_throw_idx = 1,
		.false_throw_idx = 3,
	};
	monkeys[3] = (struct monkey){
		.items = {79, 52, 55, 51},
		.num_items = 4,
		.operation = '+',
		.operant = "6",
		.test_div = 3,
		.true_throw_idx = 6,
		.false_throw_idx = 4,
	};
	monkeys[4] = (struct monkey){
		.items = {69, 92},
		.num_items = 2,
		.operation = '*',
		.operant = "17",
		.test_div = 19,
		.true_throw_idx = 7,
		.false_throw_idx = 5,
	};
	monkeys[5] = (struct monkey){
		.items = {71, 76, 73, 98, 67, 79, 99},
		.num_items = 7,
		.operation = '+',
		.operant = "8",
		.test_div = 5,
		.true_throw_idx = 0,
		.false_throw_idx = 2,
	};
	monkeys[6] = (struct monkey){
		.items = {82, 76, 69, 69, 57},
		.num_items = 5,
		.operation = '+',
		.operant = "7",
		.test_div = 11,
		.true_throw_idx = 7,
		.false_throw_idx = 4,
	};
	monkeys[7] = (struct monkey){
		.items = {65, 79, 86},
		.num_items = 3,
		.operation = '+',
		.operant = "5",
		.test_div = 17,
		.true_throw_idx = 5,
		.false_throw_idx = 0,
	};
	*m = monkeys;
	*num_monkeys = sizeof(monkeys) / sizeof(monkeys[0]);
}

void prepare_test_monkeys(struct monkey **m, int *num_monkeys)
{
	test_monkeys[0] = (struct monkey){
		.items = {79, 98},
		.num_items = 2,
		.operation = '*',
		.operant = "19",
		.test_div = 23,
		.true_throw_idx = 2,
		.false_throw_idx = 3,
	};
	test_monkeys[1] = (struct monkey){
		.items = {54, 65, 75, 74},
		.num_items = 4,
		.operation = '+',
		.operant = "6",
		.test_div = 19,
		.true_throw_idx = 2,
		.false_throw_idx = 0,
	};
	test_monkeys[2] = (struct monkey){
		.items = {79, 60, 97},
		.num_items = 3,
		.operation = '*',
		.operant = "old",
		.test_div = 13,
		.true_throw_idx = 1,
		.false_throw_idx = 3,
	};
	test_monkeys[3] = (struct monkey){
		.items = {74},
		.num_items = 1,
		.operation = '+',
		.operant = "3",
		.test_div = 17,
		.true_throw_idx = 0,
		.false_throw_idx = 1,
	};

	*m = test_monkeys;
	*num_monkeys = sizeof(test_monkeys) / sizeof(test_monkeys[0]);
}
