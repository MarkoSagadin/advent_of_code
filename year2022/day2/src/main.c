#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

FILE *f;

void swap(long *x, long *y)
{
	long tmp = *x;
	*x = *y;
	*y = tmp;
}

/* Input A/B/C and X/Y/Z, and get score outcome
 * 0 for lose
 * 3 for draw
 * 6 for win
 * */
int outcome_score_table[3][3] = {[0] = {[0] = 3, [1] = 6, [2] = 0},
				 [1] = {[0] = 0, [1] = 3, [2] = 6},
				 [2] = {[0] = 6, [1] = 0, [2] = 3}};

int get_score_outcome(char x, char y)
{
	/* Treat A and X as offsets */
	return outcome_score_table[x - 'A'][y - 'X'];
}

/* "Anyway, the second column says how the round needs to end:
 * - X means you need to lose,
 * - Y means you need to end the round in a draw, and
 * - Z means you need to win. Good luck!"
 */
/* Input A/B/C, X/Y/Z and get score of your shape response (thus avoiding useless conversion)*/
int shape_score_response_table[3][3] = {[0] = {[0] = 3, [1] = 1, [2] = 2},
					[1] = {[0] = 1, [1] = 2, [2] = 3},
					[2] = {[0] = 2, [1] = 3, [2] = 1}};

int get_score_score_response(char x, char y)
{
	return shape_score_response_table[x - 'A'][y - 'X'];
}

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	size_t num_char = 16;
	char *line = malloc(num_char);
	int answer1 = 0;
	int answer2 = 0;

	/* Steps
	 * 1.
	 * */
	while (1) {
		if (getline(&line, &num_char, f) == -1)
			break;

		char their_shape = line[0];
		char my_shape = line[2];
		/* Treat X as offset, but add one to adjust */
		answer1 += my_shape - 'X' + 1 + get_score_outcome(their_shape, my_shape);

		/* Treat X as offset, but multiplay with 3 to get outcome score. */
		char outcome = line[2];
		answer2 += ((outcome - 'X') * 3) + get_score_score_response(their_shape, outcome);
	}
	printf("Answer, part 1: %d\n", answer1);
	printf("Answer, part 2: %d\n", answer2);
}
