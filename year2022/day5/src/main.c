#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct crate_stack {
	/* Array of crates, first one is on bottom */
	char crates[128];
	/* Number of crates in stack */
	size_t num;
};

void crate_push(struct crate_stack *stack, char c)
{
	stack->crates[stack->num++] = c;
}

char crate_pop(struct crate_stack *stack)
{
	return stack->crates[--stack->num];
}

char crate_peek(struct crate_stack *stack)
{
	return stack->crates[stack->num - 1];
}

void crate_print(struct crate_stack *stack)
{
	printf("%02lu: ", stack->num);
	for (size_t i = 0; i < stack->num; i++) {
		printf("[%c]", stack->crates[i]);
	}
	printf("\n");
}

void construct_crate_stacks(struct crate_stack *crate_stacks, char input_stacks[8][64])
{
	size_t line_idx = 8;
	/* Scan each line, while adding to stacks, and then move up */
	while (line_idx--) {
		for (size_t i = 0; i < 9; i++) {
			char crate = input_stacks[line_idx][i * 4 + 1];
			if (crate != ' ') {
				crate_push(&crate_stacks[i], crate);
			}
		}
	}
}

void move_crates(struct crate_stack *crate_stacks, FILE *f, bool use_cratemover_9001)
{
	char line[64];

	int move_num;
	int src_id;
	int dst_id;

	while (fscanf(f, "move %d from %d to %d\n", &move_num, &src_id, &dst_id) == 3) {
		if (use_cratemover_9001) {
			struct crate_stack tmp = {0};

			for (size_t i = 0; i < move_num; i++) {
				char crate = crate_pop(&crate_stacks[src_id - 1]);
				crate_push(&tmp, crate);
			}
			for (size_t i = 0; i < move_num; i++) {
				char crate = crate_pop(&tmp);
				crate_push(&crate_stacks[dst_id - 1], crate);
			}
		} else {
			while (move_num--) {
				char crate = crate_pop(&crate_stacks[src_id - 1]);
				crate_push(&crate_stacks[dst_id - 1], crate);
			}
		}
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

	/* Steps for first part
	 * 1. Build the initial state of crate stacks
	 * 2. Start moving them
	 * */
	/* Read input */
	char input_stacks[8][64];
	for (size_t i = 0; i < 8; i++) {
		fgets(input_stacks[i], 64, f);
	}
	/* Fill the crate stacks, start looking from the bottom up.*/
	struct crate_stack crate_stacks[9] = {0};

	construct_crate_stacks(crate_stacks, input_stacks);

	char line[64];
	/* Throw away two lines, they are not important*/
	fgets(line, 64, f);
	fgets(line, 64, f);

	/* Move crates */
	move_crates(crate_stacks, f, false);

	printf("Answer, part 1: ");
	for (size_t i = 0; i < 9; i++) {
		printf("%c", crate_peek(&crate_stacks[i]));
	}
	printf("\n");

	/* Reset state for second part */
	rewind(f);
	for (size_t i = 0; i < 9; i++) {
		crate_stacks[i].num = 0;
	}
	construct_crate_stacks(crate_stacks, input_stacks);

	/* Steps for second part
	 * 1. Skip initial part
	 * 2. Move crates with temporary stack so that order does not change
	 * */
	for (size_t i = 0; i < 10; i++) {
		fgets(line, 64, f);
	}

	/* Move crates with CrateMover 9001. */
	move_crates(crate_stacks, f, true);

	printf("Answer, part 2: ");
	for (size_t i = 0; i < 9; i++) {
		printf("%c", crate_peek(&crate_stacks[i]));
	}
	printf("\n");
}
