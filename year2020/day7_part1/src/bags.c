/** @file bags.c
 *
 * @brief Implementation file for bags
 *
 */

#include <stdlib.h>
#include <string.h>

#include "bags.h"
#include "file_utility.h"

static char *prv_get_inner_bag_color(char *sub_string)
{
	/* Small fix incase the input string contains space on the first place. */
	if (*sub_string == ' ') {
		sub_string++;
	}

	int inner_bag_color_len = get_len_sub_words(sub_string, 1, 3) + 1;
	char *inner_bag_color = malloc(inner_bag_color_len * sizeof(char));
	get_sub_words(sub_string, inner_bag_color, 1, 3);

	return inner_bag_color;
}

void bag_create(struct bag *bag, char *line)
{
	char temp_string[128];

	int color_name_len = get_len_sub_words(line, 0, 2) + 1;

	bag->color = malloc(color_name_len * sizeof(char));
	get_sub_words(line, bag->color, 0, 2);

	/* Get all words after word 'contain' which is always 4th word. */
	int max_num_words = get_num_tokens_in_line(line, ' ');
	get_sub_words(line, temp_string, 4, max_num_words);

	/* Check if we contain any inner bags. */
	if (strcmp("no other bags.", temp_string) == 0) {
		bag->inner_bags_num = 0;
		bag->inner_bags_list = NULL;
		return;
	}

	bag->inner_bags_num = get_num_tokens_in_line(temp_string, ',');

	/* Create 2d array of strings for inner bags*/
	bag->inner_bags_list = malloc(bag->inner_bags_num * sizeof(char *));

	/* Start splitting them, for each token that you get
	 * save it into 2d array */
	char *ptr = strtok(temp_string, ",");

	bag->inner_bags_list[0] = prv_get_inner_bag_color(ptr);

	for (int i = 1; i < bag->inner_bags_num; ++i) {
		ptr = strtok(NULL, ",");
		bag->inner_bags_list[i] = prv_get_inner_bag_color(ptr);
	}
}

bool bag_contains_no_bags(struct bag *bag)
{
	return !bag->inner_bags_num ? true : false;
}

bool bag_contains_gold_bag(struct bag *bag)
{
	if (0 == bag->inner_bags_num) {
		return false;
	}
	for (int i = 0; i < bag->inner_bags_num; i++) {
		if (0 == strcmp("shiny gold", bag->inner_bags_list[i])) {
			return true;
		}
	}
	return false;
}

void get_correct_bag_line(FILE *f, int num_file_lines, char *search_string, char **match_line,
			  int *match_line_index)
{
	char *line;
	char *bag_color;
	for (int n = 0; n < num_file_lines; n++) {
		int line_len = get_num_chars_in_nth_line(f, n) + 1;
		line = malloc(line_len * sizeof(char));

		get_nth_line(f, line, line_len, n);

		int bag_color_len = get_len_sub_words(line, 0, 2) + 1;
		bag_color = malloc(bag_color_len * sizeof(char));

		get_sub_words(line, bag_color, 0, 2);
		if (strcmp(bag_color, search_string) == 0) {
			free(bag_color);
			*match_line = line;
			*match_line_index = n;
			return;
		}
		free(line);
		free(bag_color);
	}
	*match_line = NULL;
	return;
}

void fill_bag_data_list(FILE *f, struct bag_data *list, int list_size)
{
	char line;

	for (int n = 0; n < list_size; n++) {
		char *bag_color = get_sub_words_in_nth_line(f, n, 0, 2);
		list[n].bag_color = bag_color;
		list[n].searched = false;
		list[n].contains_gold = false;
	}
}

bool bag_traverse(FILE *f, char *line, int bag_index, int list_size, struct bag_data *list)
{
	printf("\nEntered bag, line index: %u\n", bag_index);

	/* Do not continue search, if this bag was already searched, this saves
	 * time. */
	if (list[bag_index].searched) {
		return list[bag_index].contains_gold;
	}
	list[bag_index].searched = true;

	/* Create bag from input line. */
	struct bag *current_bag = malloc(sizeof(struct bag));
	bag_create(current_bag, line);

	/* If it contains no bags, then the search is done, just exit. */
	if (bag_contains_no_bags(current_bag)) {
		free(current_bag);
		list[bag_index].searched = false;
		return list[bag_index].contains_gold;
	}

	/* Check contents of the bag before continuing search. */
	if (bag_contains_gold_bag(current_bag)) {
		list[bag_index].contains_gold = true;
	}

	for (int i = 0; i < current_bag->inner_bags_num; i++) {
		/* First find the correct line and its index in the FILE*/
		int bag_line_index;
		char *bag_line;

		get_correct_bag_line(f, list_size, current_bag->inner_bags_list[i], &bag_line,
				     &bag_line_index);

		/* Do the traverse on the inner bag, if any of the below bags will
		 * contain shiny gold, return true. */
		list[bag_line_index].contains_gold =
			bag_traverse(f, bag_line, bag_line_index, list_size, list);

		/* Here is a tricky part, if one of the inner bags contains the shiny
		 * gold bag pass that info to the bag that contains it. */
		if (list[bag_line_index].contains_gold) {
			list[bag_index].contains_gold = true;
		}
		free(bag_line);
	}

	/* Iterated through all inner bags, but none contained the shiny gold bag.
	 * go one level back. */
	free(current_bag);
	return list[bag_index].contains_gold;
}

/*** end of file ***/
