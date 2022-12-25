#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum type {
	LIST,
	INTEGER,
};

#define MIN(a, b) ((a) < (b) ? (a) : (b))

struct element {
	/* Determines which members can be operated on. */
	enum type type;

	/* List type members, here we have a list of references to the elements, so we avoid
	 * copying the entire lists */
	struct element **list;
	size_t len;

	/* Integer type member */
	int value;
};

struct element *list_create(void)
{
	struct element *list = malloc(sizeof(struct element));

	list->len = 0;
	list->type = LIST;
	list->list = NULL;
	list->value = 0;

	return list;
}

void list_add_integer(struct element *list, int num)
{
	list->list = realloc(list->list, (list->len + 1) * sizeof(struct element));
	struct element *integer = malloc(sizeof(struct element));

	integer->list = NULL;
	integer->len = 0;
	integer->value = num;
	integer->type = INTEGER;

	list->list[list->len++] = integer;
}

void list_add_item(struct element *list, struct element *item)
{
	list->list = realloc(list->list, (list->len + 1) * sizeof(struct element));
	list->list[list->len++] = item;
}

void convert_integer_to_list(struct element *element)
{
	list_add_integer(element, element->value);
	element->type = LIST;
}

/* If 1, then left integer is smaller than the right one OR left list ran out of items first.
 * If -1, then left integer is greater than the right one OR right list ran out of items first.
 * If 0, then left and right integers are the sample OR left and right list are the same lenght and
 * their value comparisons could not make an decision about order.
 */
int compare_elements(struct element *left, struct element *right)
{
	if (left->type == right->type && INTEGER == left->type) {
		/* Integer comparison */
		if (left->value == right->value)
			return 0;
		else if (left->value < right->value)
			return -1;
		else
			return 1;
	} else if (left->type == right->type && LIST == left->type) {
		/* List comparison */
		size_t min_num_elements = MIN(left->len, right->len);
		for (size_t i = 0; i < min_num_elements; i++) {
			int result = compare_elements(left->list[i], right->list[i]);
			if (result != 0)
				return result;
		}
		if (left->len == right->len) {
			return 0;
		} else if (left->len == min_num_elements) {
			return -1;
		} else {
			return 1;
		}
	} else {
		/* Different types, covert before comparing */
		convert_integer_to_list(left->type == INTEGER ? left : right);
		return compare_elements(left, right);
	}
}

int sort_compare_elements(const void *left, const void *right)
{
	return compare_elements(*(struct element **)left, *(struct element **)right);
}

void print_element_recur(struct element *element)
{
	if (element->type == LIST) {
		printf("[");
		for (size_t i = 0; i < element->len; i++) {
			struct element *item = element->list[i];
			print_element_recur(item);
			if (i < element->len - 1)
				printf(",");
		}
		printf("]");
	} else {
		printf("%i", element->value);
	}
}

void print_element(struct element *element)
{
	print_element_recur(element);
	printf("\n");
}

void parse_element(struct element **element, char **line)
{
	char *c = *line;

	/* Loop until newline or end of list*/
	for (;; c++) {
		switch (*c) {
		case '[': {
			struct element *list = list_create();
			/* Move once character forward to avoid parsing "[" in the loop. */
			c++;
			parse_element(&list, &c);
			if (*element == NULL) {
				*element = list;
			} else {
				list_add_item(*element, list);
			}
		} break;
		case ',':
			/* Don't do nothing with this. */
			break;
		case ']':
		case '\n':
			/* Assing back c, to give the info of the progress back to the caller. */
			*line = c;
			return;
			break;
		default: {
			/* A number */
			char *endptr = NULL;
			int num = strtol(c, &endptr, 10);
			/* Skip the parsed number (in case that it has more than one digit), but
			 * move back for one so that loop logic can be simpler.  */
			c = endptr - 1;
			list_add_integer(*element, num);
		} break;
		}
	};
}

struct element *get_element_from_line(FILE *f)
{
	char *line = NULL;
	size_t char_read = 0;
	struct element *element = NULL;

	getline(&line, &char_read, f);
	parse_element(&element, &line);

	return element;
}

bool empty_line_check(FILE *f)
{
	char *line = NULL;
	size_t char_read = 0;

	ssize_t rc = getline(&line, &char_read, f);

	return rc == -1 ? false : true;
}

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	FILE *f = fopen("src/input.txt", "r");
	if (!f) {
		printf("No file found!\n");
		return 0;
	}

	/* Originally there are 300 packets, add 2 for two divider packets*/
	struct element *packets[302];
	int pack_idx = 0;

	struct element *left;
	struct element *right;
	int result;

	int pair_idx = 0;
	int sum_pair_idx = 0;

	while (1) {
		left = get_element_from_line(f);
		right = get_element_from_line(f);

		result = compare_elements(left, right);
		pair_idx++;

		if (result == -1) {
			sum_pair_idx += pair_idx;
		}

		/* Add for later sorting */
		packets[pack_idx++] = left;
		packets[pack_idx++] = right;

		if (!empty_line_check(f)) {
			break;
		}
	}
	/* add divider packets */
	char *div_packet_str[] = {"[[2]]\n", "[[6]]\n"};
	struct element *div_packet[2] = {0};
	for (size_t i = 0; i < 2; i++) {
		parse_element(&div_packet[i], &div_packet_str[i]);
		packets[pack_idx++] = div_packet[i];
	}

	/* Sort all packets */
	qsort(packets, pack_idx, sizeof(struct element *), sort_compare_elements);

	/* Find divider packets */
	int div_packet_idx[2];
	for (int i = 0; i < pack_idx; i++) {
		for (size_t j = 0; j < 2; j++) {
			if (memcmp(div_packet[j], packets[i], sizeof(struct element)) == 0) {
				div_packet_idx[j] = i + 1;
			}
		}
	}

	printf("Answer 1: %i\n", sum_pair_idx);
	printf("Answer 2: %i\n", div_packet_idx[0] * div_packet_idx[1]);
}
