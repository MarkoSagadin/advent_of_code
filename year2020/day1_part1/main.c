#include <stdio.h>
#include <time.h>
#include <stdint.h>

#define MOD_MAX_FILTER_LEN 20
typedef struct {
    uint16_t buffer[MOD_MAX_FILTER_LEN];
    uint8_t head;
    uint8_t max_len;
    uint8_t len;
    uint32_t accumulator;
} mod_filter_t;

void mod_filter_init(mod_filter_t *filter, uint8_t len)
{
    //assert leng ehre

    filter->accumulator = 0;
    filter->head = 0;
    filter->max_len = len;
    filter->len = 0;

    for(int i = 0; i < len; i++) {
            filter->buffer[i] = 0;
    }
}


void mod_filter_insert(mod_filter_t *filter, uint16_t current_value)
{
    /* Add current value */
    filter->accumulator += current_value;

    /* Increase buffer len until max is reached, */
    if (filter->len < filter->max_len) {
        filter->len++;
        /* Add current value */
        filter->buffer[filter->head] = current_value;

        /* Increment head index, until we hit the end of array, then reset */
        if (filter->head < filter->len - 1) {
            filter->head++;
        }
        else {
            filter->head = 0;
        }
        return;
    }

    /* Remove oldest value */
    filter->accumulator -= filter->buffer[filter->head];

    /* Add current value */
    filter->buffer[filter->head] = current_value;
    
    /* Increment head index, until we hit the end of array, then reset */
    if (filter->head < filter->len - 1) {
        filter->head++;
    }
    else {
        filter->head = 0;
    }
}

int32_t util_round(double number)
{
    return (number >= 0) ? (int32_t)(number + 0.5) : (int32_t)(number - 0.5);
}

float mod_filter_get_avg(mod_filter_t filter)
{
    if (filter.len == 0) {
        return 0;
    }
    printf("ACCU: %u, len: %u\n", filter.accumulator, filter.len); 
    //return (uint16_t)util_round((float)filter.accumulator / filter.len);
    return (float)filter.accumulator / filter.len;
}

int main()
{
    mod_filter_t filter;
    mod_filter_init(&filter, 5);


    mod_filter_insert(&filter, 1);
    printf("avg: %f\n", mod_filter_get_avg(filter));
    mod_filter_insert(&filter, 2);
    printf("avg: %f\n", mod_filter_get_avg(filter));
    mod_filter_insert(&filter, 3);
    printf("avg: %f\n", mod_filter_get_avg(filter));
    mod_filter_insert(&filter, 4);
    printf("avg: %f\n", mod_filter_get_avg(filter));
    mod_filter_insert(&filter, 5);
    printf("avg: %f\n", mod_filter_get_avg(filter));
    mod_filter_insert(&filter, 5);
    printf("avg: %f\n", mod_filter_get_avg(filter));
    mod_filter_insert(&filter, 5);
    printf("avg: %f\n", mod_filter_get_avg(filter));
    return 0;
}
