#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "bags.h"

FILE *f;

int main()
{
    /* Open file, file path is a bit weird due to the way the executable is run */
    // f = fopen("../src/input.txt","r");
    // if (f == NULL) {
    //     printf("No file found!\n");
    //     return 0;
    // }

    // /* Count number of lines in a file */
    // int num_lines = get_num_lines(f);
    // printf("Num lines is %d\n", num_lines);

    // /* Create big enough tree of bags*/
    // struct bag ** bags = malloc(num_lines * sizeof(struct bag *)); 

    // /* Count number of chars in first line */
    // int num_chars = get_num_chars_in_line(f);
    // printf("Num chars is %d\n\n", num_chars);


    // /* Copy entire content of the file into array */
    // copy_file_into_array(f, forest);
    char * line = "light red bags contain "
                  "1 bright white bag, 2 muted yellow bags.\n";
    char * expected_bag_color = "light red";

    struct bag bag = { 0 };

    bag_create(&bag, line);
    
    printf("bag color: %s", bag.color);
}
