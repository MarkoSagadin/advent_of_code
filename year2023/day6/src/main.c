#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct input {
	int time;
	int dist;
};

int calc(double time, double dist)
{
	/* Basic quadratic formula solution, however some fiddling was needed with the the rounding.
	 */
	int x1 = floor((-time + sqrt(time * time - 4.0 * dist)) / (-2.0) + 1.0);
	int x2 = ceil((-time - sqrt(time * time - 4.0 * dist)) / (-2.0)) - 0.99;

	return x2 - x1 + 1;
}

int main()
{
	struct input inputs[] = {{35, 212}, {93, 2060}, {73, 1201}, {66, 1044}};
	// struct input inputs[] = {{7, 9}, {15, 40}, {30, 200}};

	int result = 1;
	for (size_t i = 0; i < 4; i++) {
		int res = calc(inputs[i].time, inputs[i].dist);
		result *= res;
	}

	printf("Part 1 is: %i\n", result);
	printf("Part 2 is: %i\n", calc(35937366, 212206012011044));
}
