#include "unity.h"
#include "file_utility.h"
#include <stdio.h>
#include <stdlib.h>

FILE *f;

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
    /* Weird path, due to the way ceelding is called*/
    f = fopen("test/test_input.txt","r"); 
    if (f == NULL) {
        printf("No file found!\n");
    }
}

void tearDown(void) 
{
    // clean stuff up here
    fclose(f);
}

void test_get_num_chars_in_0th_line(void) 
{
    int num_chars = get_num_chars_in_nth_line(f, 0);

    TEST_ASSERT_EQUAL_INT(64, num_chars);
}

void test_get_num_chars_in_1st_line(void) 
{
    int num_chars = get_num_chars_in_nth_line(f, 1);

    TEST_ASSERT_EQUAL_INT(67, num_chars);
}

void test_get_num_chars_in_the_last_line(void) 
{
    int num_chars = get_num_chars_in_nth_line(f, 8);

    TEST_ASSERT_EQUAL_INT(41, num_chars);
}

void test_get_0th_line(void) 
{
    int num_chars = get_num_chars_in_nth_line(f, 0) + 1;
    char * line = malloc(num_chars * sizeof(char));
    char * expected_line = "light red bags contain 1 bright white bag, "
                           "2 muted yellow bags.\n";
    
    if (line == NULL) {
        TEST_FAIL_MESSAGE("Not enough memory");
    }

    get_nth_line(f, line, num_chars, 0);

    TEST_ASSERT_EQUAL_STRING(expected_line, line);
}


void test_get_4th_line(void) 
{
    int num_chars = get_num_chars_in_nth_line(f, 4) + 1;
    char * line = malloc(num_chars * sizeof(char));
    char * expected_line = "shiny gold bags contain 1 dark olive bag, "
                           "2 vibrant plum bags.\n";
    
    if (line == NULL) {
        TEST_FAIL_MESSAGE("Not enough memory");
    }

    get_nth_line(f, line, num_chars, 4);

    TEST_ASSERT_EQUAL_STRING(expected_line, line);
}

void test_get_len_of_first_two_words(void) 
{
    char * line = "light red bags contain 1 bright white bag, "
                   "2 muted yellow bags.\n";
    int len = get_len_sub_words(line, 0, 2);

    TEST_ASSERT_EQUAL_INT(9, len);
}

void test_get_len_of_first_two_words_twice(void) 
{
    char * line = "light red bags contain 1 bright white bag, "
                   "2 muted yellow bags.\n";
    int len = get_len_sub_words(line, 0, 2);
    len = get_len_sub_words(line, 0, 2);

    TEST_ASSERT_EQUAL_INT(9, len);
}

void test_get_len_of_second_word(void) 
{
    char * line = "light red bags contain 1 bright white bag, "
                   "2 muted yellow bags.\n";
    int len = get_len_sub_words(line, 1, 2);

    TEST_ASSERT_EQUAL_INT(3, len);
}

void test_get_first_two_words(void) 
{
    char * line = "light red bags contain 1 bright white bag, "
                   "2 muted yellow bags.\n";
    int len = get_len_sub_words(line, 0, 2) + 1;
    char * actual_words = malloc(len * sizeof(char));
    char * expected_words = "light red";
    
    get_sub_words(line, actual_words, 0, 2);

    TEST_ASSERT_EQUAL_STRING(expected_words, actual_words);
}

void test_get_first_eight_words(void) 
{
    char * line = "light red bags contain 1 bright white bag, "
                   "2 muted yellow bags.\n";
    int len = get_len_sub_words(line, 0, 8) + 1;
    char * actual_words = malloc(len * sizeof(char));
    char * expected_words = "light red bags contain 1 bright white bag,";
    
    get_sub_words(line, actual_words, 0, 8);

    TEST_ASSERT_EQUAL_STRING(expected_words, actual_words);
}

void test_get_middle_words(void) 
{
    char * line = "light red bags contain 1 bright white bag, "
                   "2 muted yellow bags.\n";
    int len = get_len_sub_words(line, 2, 8) + 1;
    char * actual_words = malloc(len * sizeof(char));
    char * expected_words = "bags contain 1 bright white bag,";
    
    get_sub_words(line, actual_words, 2, 8);

    TEST_ASSERT_EQUAL_STRING(expected_words, actual_words);
}
