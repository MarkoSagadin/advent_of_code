#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bags.h"
#include "file_utility.h"

FILE *f;

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	int list_size = get_num_lines(f);
	int gold_bag_counter = 0;
	char current_line[256] = {0};

	/* Create and fill list */
	struct bag_data *list = malloc(list_size * sizeof(struct bag_data));
	fill_bag_data_list(f, list, list_size);

	for (int i = 0; i < list_size; i++) {
		printf("Starting search on line: %u", i);
		get_nth_line(f, current_line, 256, i);

		list[i].contains_gold = bag_traverse(f, current_line, i, list_size, list);
	}

	/* Count all bags where shiny golden bag could be present. */
	for (int i = 0; i < list_size; i++) {
		if (list[i].contains_gold) {
			printf("BAG index: %u\n", i);
			gold_bag_counter++;
		}
	}
	printf("number of bags that contain shiny gold bag: %u", gold_bag_counter);
}
