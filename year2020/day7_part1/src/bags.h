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

struct bag {
    char * color;
    struct bag * outer_bag;
    int inner_bags_num;
    char ** inner_bags_list;
};

void bag_create(struct bag * bag, char * line);
bool bag_contains_no_bags(struct bag * bag);
bool bag_contains_gold_bag(struct bag * bag);

#ifdef __cplusplus
}
#endif

#endif /* BAGS_H */
/*** end of file ***/
