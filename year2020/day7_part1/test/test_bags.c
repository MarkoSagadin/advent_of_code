#include "unity.h"
#include "bags.h"
#include "file_utility.h"
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
    char * line = "light red bags contain "
                  "1 bright white bag, 2 muted yellow bags.\n";
    char * expected_bag_color = "light red";

    struct bag bag = { 0 };

    bag_create(&bag, line);
    
    TEST_ASSERT_EQUAL_STRING(expected_bag_color, bag.color);
}

void test_create_empty_bag_from_line(void)
{
    char * line = "light red bags contain no other bags.\n";

    struct bag bag = { 0 };
    bag_create(&bag, line);
    
    TEST_ASSERT(bag_contains_no_bags(&bag));
}

void test_create_non_empty_bag_from_line(void)
{
    char * line = "light red bags contain "
                  "1 bright white bag, 2 muted yellow bags.\n";

    struct bag bag = { 0 };
    bag_create(&bag, line);
    
    TEST_ASSERT_FALSE(bag_contains_no_bags(&bag));
}

void test_create_bag_with_two_inner_bags(void)
{
    char * line = "light red bags contain "
                  "1 bright white bag, 2 muted yellow bags.\n";

    struct bag bag = { 0 };
    bag_create(&bag, line);
    
    TEST_ASSERT_EQUAL_INT(2, bag.inner_bags_num);
}

void test_create_bag_with_4_inner_bags(void)
{
    char * line = "pale cyan bags contain 4 light olive bags, 2 dull lime bags,"
                  " 4 faded black bags, 4 plaid red bags.\n";

    struct bag bag = { 0 };
    bag_create(&bag, line);
    
    TEST_ASSERT_EQUAL_INT(4, bag.inner_bags_num);
}

void test_two_inner_bag_color_check(void)
{
    char * line = "light red bags contain "
                  "1 bright white bag, 2 muted yellow bags.\n";

    struct bag bag = { 0 };
    bag_create(&bag, line);
    
    TEST_ASSERT_EQUAL_STRING("bright white", bag.inner_bags_list[0]);
    TEST_ASSERT_EQUAL_STRING("muted yellow", bag.inner_bags_list[1]);
}

void test_four_inner_bag_color_check(void)
{
    char * line = "pale cyan bags contain 4 light olive bags, 2 dull lime bags,"
                  " 4 faded black bags, 4 plaid red bags.\n";
    char * expected_inner_bags[4] = { "light olive",
                                      "dull lime",
                                      "faded black",
                                      "plaid red" };
    struct bag bag = { 0 };
    bag_create(&bag, line);
    
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_STRING(expected_inner_bags[i], 
                                 bag.inner_bags_list[i]);
    }
}

void test_gold_bag_check(void)
{
    char * line = "bright white bags contain 1 shiny gold bag.\n";

    struct bag bag = { 0 };
    bag_create(&bag, line);
    
    TEST_ASSERT(bag_contains_gold_bag(&bag));
}

void test_gold_bag_negative(void)
{
    char * line = "light red bags contain "
                  "1 bright white bag, 2 muted yellow bags.\n";

    struct bag bag = { 0 };
    bag_create(&bag, line);
    
    TEST_ASSERT_FALSE(bag_contains_gold_bag(&bag));
}

void test_gold_bag_negative_empty(void)
{
    char * line = "light red bags contain no other bags.\n";

    struct bag bag = { 0 };
    bag_create(&bag, line);
    
    TEST_ASSERT_FALSE(bag_contains_gold_bag(&bag));
}

