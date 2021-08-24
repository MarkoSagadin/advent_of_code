/** @file bags.c
 * 
 * @brief Implementation file for bags
 *
 */

#include <string.h>
#include <stdlib.h>

#include "file_utility.h"
#include "bags.h"

static char * prv_get_inner_bag_color(char * sub_string)
{
    /* Small fix incase the input string contains space on the first place. */
    if (*sub_string == ' ') {
        sub_string++;
    }

    int inner_bag_color_len = get_len_sub_words(sub_string, 1, 3) + 1;
    char * inner_bag_color = malloc(inner_bag_color_len * sizeof(char));
    get_sub_words(sub_string, inner_bag_color, 1, 3);

    return inner_bag_color;
}


void bag_create(struct bag * bag, char * line)
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
    char * ptr = strtok(temp_string, ",");

    bag->inner_bags_list[0] = prv_get_inner_bag_color(ptr);

    for (int i = 1; i < bag->inner_bags_num; ++i) {
        ptr = strtok(NULL, ",");
        bag->inner_bags_list[i] = prv_get_inner_bag_color(ptr);
    }
}

bool bag_contains_no_bags(struct bag * bag)
{
    return !bag->inner_bags_num ? true : false;
}

bool bag_contains_gold_bag(struct bag * bag)
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

/*** end of file ***/
