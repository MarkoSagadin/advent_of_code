#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static inline int calc_strength(int cycles, int reg_x)
{
	return ((cycles - 20) % 40) == 0 ? cycles * reg_x : 0;
}

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	FILE *f = fopen("src/input.txt", "r");
	if (!f) {
		printf("No file found!\n");
		return 0;
	}

	int reg_x = 1;
	int cycles = 0;
	int answer1 = 0;

	char *cmd = NULL;
	int value;

	printf("\nAnswer, part 2:\n\n");
	while (fscanf(f, "%ms %i", &cmd, &value) != EOF) {

		/* Control here how many cycles we have to do, depending if it is noop or addx */
		int repeat_num = strcmp(cmd, "noop") == 0 ? 1 : 2;

		/* Increase cycles */
		for (size_t i = 0; i < repeat_num; i++) {
			cycles++;
			answer1 += calc_strength(cycles, reg_x);

			if (((cycles - 1) % 40) >= (reg_x - 1) &&
			    ((cycles - 1) % 40) <= (reg_x + 1)) {
				printf("#");
			} else {
				printf(".");
			}

			/* Add new line after every 40 cycles */
			if (!(cycles % 40)) {
				printf("\n");
			}
		}

		/* End of cycle, update reg X if we had addx command */
		if (repeat_num == 2) {
			reg_x += value;
		}
	}

	printf("\nAnswer, part 1: %i\n", answer1);
	// printf("Answer, part 2: %i\n", answer2);
}
