/** @file bags.h
 *
 * @brief Interface file for bags
 *
 */

#ifndef BAGS_H
#define BAGS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include <stdio.h>

struct bag {
	char *color;
	struct bag *outer_bag;
	int inner_bags_num;
	char **inner_bags_list;
};

struct bag_data {
	char *bag_color;
	bool searched;
	bool contains_gold;
};

void bag_create(struct bag *bag, char *line);
bool bag_contains_no_bags(struct bag *bag);
bool bag_contains_gold_bag(struct bag *bag);
/* Recursive function, when calling always set outer_bag to NULL. */
bool bag_traverse(FILE *f, char *line, int bag_index, int list_size, struct bag_data *list);

void get_correct_bag_line(FILE *f, int num_file_lines, char *search_string, char **match_line,
			  int *match_line_index);

void fill_bag_data_list(FILE *f, struct bag_data *list, int list_size);

#ifdef __cplusplus
}
#endif

#endif /* BAGS_H */
/*** end of file ***/
