#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *f;

static void fill_array(char *line, int *numbers, int *num)
{
	*num = 0;
	while (1) {
		char *next;
		numbers[(*num)++] = strtol(line, &next, 10);
		next++;

		if (*next == '|' || *next == '\0') {
			return;
		}
		line = next;
	}
}

int main()
{
	f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	long sum1 = 0;
	long sum2 = 0;
	int cur_card = 0;

	/* Take account the original cards. */
	int card_copy_counter[204];
	for (int i = 0; i < 204; i++) {
		card_copy_counter[i] = 1;
	}

	while (1) {
		int winning_numbers[50];
		int my_numbers[50];
		int num_win_num, num_my_num;

		size_t num_char = 0;
		char *line = NULL;
		if (getline(&line, &num_char, f) == -1)
			break;

		/* Move until ':' */
		while (*line++ != ':') {
		}
		fill_array(line, winning_numbers, &num_win_num);

		/* Move until '|' */
		while (*line++ != '|') {
		}
		fill_array(line, my_numbers, &num_my_num);

		int point = 0;
		int num_match = 0;

		for (int i = 0; i < num_win_num; i++) {
			for (int j = 0; j < num_my_num; j++) {
				if (winning_numbers[i] == my_numbers[j]) {
					point = point == 0 ? 1 : point * 2;
					num_match++;
				}
			}
		}

		for (int i = cur_card + 1; i < cur_card + num_match + 1; i++) {
			card_copy_counter[i] += card_copy_counter[cur_card];
		}

		sum1 += point;
		cur_card++;
	}

	for (int i = 0; i < cur_card; i++) {
		sum2 += card_copy_counter[i];
	}

	printf("Part 1 is: %lu\n", sum1);
	printf("Part 2 is: %lu\n", sum2);
}
