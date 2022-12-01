#include "bags.h"
#include "file_utility.h"
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>

/*
Contents of test_input.txt:

light red bags contain 1 bright white bag, 2 muted yellow bags.
dark orange bags contain 3 bright white bags, 4 muted yellow bags.
bright white bags contain 1 shiny gold bag.
muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.
shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.
dark olive bags contain 3 faded blue bags, 4 dotted black bags.
vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.
faded blue bags contain no other bags.
dotted black bags contain no other bags.
*/

void setUp(void)
{
	// set stuff up here
}

void tearDown(void)
{
	// clean stuff up here
}

void test_create_bag_from_line(void)
{
	char *line = "light red bags contain "
		     "1 bright white bag, 2 muted yellow bags.\n";
	char *expected_bag_color = "light red";

	struct bag bag = {0};

	bag_create(&bag, line);

	TEST_ASSERT_EQUAL_STRING(expected_bag_color, bag.color);
}

void test_create_empty_bag_from_line(void)
{
	char *line = "light red bags contain no other bags.\n";

	struct bag bag = {0};
	bag_create(&bag, line);

	TEST_ASSERT(bag_contains_no_bags(&bag));
}

void test_create_non_empty_bag_from_line(void)
{
	char *line = "light red bags contain "
		     "1 bright white bag, 2 muted yellow bags.\n";

	struct bag bag = {0};
	bag_create(&bag, line);

	TEST_ASSERT_FALSE(bag_contains_no_bags(&bag));
}

void test_create_bag_with_two_inner_bags(void)
{
	char *line = "light red bags contain "
		     "1 bright white bag, 2 muted yellow bags.\n";

	struct bag bag = {0};
	bag_create(&bag, line);

	TEST_ASSERT_EQUAL_INT(2, bag.inner_bags_num);
}

void test_create_bag_with_4_inner_bags(void)
{
	char *line = "pale cyan bags contain 4 light olive bags, 2 dull lime bags,"
		     " 4 faded black bags, 4 plaid red bags.\n";

	struct bag bag = {0};
	bag_create(&bag, line);

	TEST_ASSERT_EQUAL_INT(4, bag.inner_bags_num);
}

void test_two_inner_bag_color_check(void)
{
	char *line = "light red bags contain "
		     "1 bright white bag, 2 muted yellow bags.\n";

	struct bag bag = {0};
	bag_create(&bag, line);

	TEST_ASSERT_EQUAL_STRING("bright white", bag.inner_bags_list[0]);
	TEST_ASSERT_EQUAL_STRING("muted yellow", bag.inner_bags_list[1]);
}

void test_four_inner_bag_color_check(void)
{
	char *line = "pale cyan bags contain 4 light olive bags, 2 dull lime bags,"
		     " 4 faded black bags, 4 plaid red bags.\n";
	char *expected_inner_bags[4] = {"light olive", "dull lime", "faded black", "plaid red"};
	struct bag bag = {0};
	bag_create(&bag, line);

	for (int i = 0; i < 4; i++) {
		TEST_ASSERT_EQUAL_STRING(expected_inner_bags[i], bag.inner_bags_list[i]);
	}
}

void test_gold_bag_check(void)
{
	char *line = "bright white bags contain 1 shiny gold bag.\n";

	struct bag bag = {0};
	bag_create(&bag, line);

	TEST_ASSERT(bag_contains_gold_bag(&bag));
}

void test_gold_bag_negative(void)
{
	char *line = "light red bags contain "
		     "1 bright white bag, 2 muted yellow bags.\n";

	struct bag bag = {0};
	bag_create(&bag, line);

	TEST_ASSERT_FALSE(bag_contains_gold_bag(&bag));
}

void test_gold_bag_negative_empty(void)
{
	char *line = "light red bags contain no other bags.\n";

	struct bag bag = {0};
	bag_create(&bag, line);

	TEST_ASSERT_FALSE(bag_contains_gold_bag(&bag));
}

void test_search_for_bag(void)
{
	FILE *f = fopen("test/test_input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
	}
	int list_size = get_num_lines(f);
	int match_line_index = 0;
	char *actual_line;

	get_correct_bag_line(f, list_size, "light red", &actual_line, &match_line_index);

	char *expected_line = "light red bags contain 1 bright white bag,"
			      " 2 muted yellow bags.\n";

	TEST_ASSERT_EQUAL_STRING(expected_line, actual_line);
}

void test_search_for_bag_not_found(void)
{
	FILE *f = fopen("test/test_input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
	}
	int list_size = get_num_lines(f);
	int match_line_index = 0;
	char *actual_line;

	get_correct_bag_line(f, list_size, "muted red", &actual_line, &match_line_index);

	char *expected_line = NULL;

	TEST_ASSERT_EQUAL_INT(expected_line, actual_line);
}

void test_construct_bag_data_list(void)
{
	FILE *f = fopen("test/test_input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
	}
	int list_size = get_num_lines(f);

	struct bag_data *list = malloc(list_size * sizeof(struct bag_data));
	fill_bag_data_list(f, list, list_size);

	char *expected_list[] = {"light red",	 "dark orange", "bright white",
				 "muted yellow", "shiny gold",	"dark olive",
				 "vibrant plum", "faded blue",	"dotted black"};

	for (int i = 0; i < list_size; i++) {
		TEST_ASSERT_EQUAL_STRING(expected_list[i], list[i].bag_color);
	}
}

void test_run_test_input(void)
{
	/* Weird path, due to the way ceelding is called*/
	FILE *f = fopen("test/test_input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
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

	TEST_ASSERT_EQUAL_INT(4, gold_bag_counter);
}
