#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* What I need to implement:
 * 1. Part
 * Parsing function. Create an list of nodes and fill node, left, right members
 * Connecting function. For each node use left and right members to find the correct node in the
 * list of nodes and connect the pointers.
 * Step function. Given a node and a 'L' or 'R' step it returns the next node.
 *
 * Run step function in the loop, counting steps, checking when you have to reset the step index and
 * checking for the ZZZ.
 *
 * 2. Part
 * Find all nodes ending with with A and add them to a list.
 * Implement a step function that does the same step on all nodes in the lists.
 * Implement a verify function that checks each node in the list if it ends with an Z.
 * */

struct node {
	char *node;
	char *left;
	char *right;
	struct node *left_n;
	struct node *right_n;
};

struct network {
	struct node *nodes;
	size_t num;
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

void network_append_node(struct network *net, struct node *node)
{
	/* Create space  */
	net->nodes = realloc(net->nodes, (net->num + 1) * sizeof(struct node));
	/* Copy contents into new space */
	net->nodes[net->num++] = *node;
}

void print_network(struct network *net)
{
	for (size_t i = 0; i < net->num; i++) {
		struct node *n = &net->nodes[i];
		printf("node[%lu]: %s left=%s, right=%s\n", i, n->node, n->left, n->right);
	}
}

void parse_network_from_file(FILE *f, struct network *net)
{
	net->nodes = NULL;
	net->num = 0;

	while (1) {
		char *line = NULL;
		size_t num_char;
		if (getline(&line, &num_char, f) == -1) {
			break;
		}

		/* Parse node from the line */
		struct node n;

		sscanf(line, "%3ms = (%3ms, %3ms)", &n.node, &n.left, &n.right);

		network_append_node(net, &n);
	}
}

struct node *network_find_node(struct network *net, char *label)
{
	for (size_t i = 0; i < net->num; i++) {
		/* On the match return the node poiner */
		if (!strcmp(label, net->nodes[i].node)) {
			return &net->nodes[i];
		}
	}

	printf("Shouldn't happen\n");
	exit(0);
	return NULL;
}

/* Connecting function. For each node use left and right members to find the correct node in the
 * list of nodes and connect the pointers.
 */
void network_connect(struct network *net)
{
	for (size_t i = 0; i < net->num; i++) {
		struct node *n = &net->nodes[i];

		n->left_n = network_find_node(net, n->left);
		n->right_n = network_find_node(net, n->right);
	}
}

struct node *node_step(struct node *node, char dir)
{
	return dir == 'L' ? node->left_n : node->right_n;
}

int solve_first_part(struct network *net, char *instructions)
{
	int steps = 0;
	char *cur_inst = instructions;

	struct node *cur = network_find_node(net, "AAA");

	// Run step function in the loop, counting steps, checking when you have to reset the step
	// index and checking for the ZZZ.
	while (1) {
		cur = node_step(cur, *cur_inst++);
		steps++;
		// printf("Stepped to: %s\n", cur->node);

		/* Did we reach the goal? Then we stop stepping. */
		if (!strcmp(cur->node, "ZZZ")) {
			return steps;
		}

		/* Did we reach the end of instructions? Then we start from the beginning. */
		if (*cur_inst == '\n') {
			// printf("Reseting\n");
			cur_inst = instructions;
		}
	}
}

void network_collect_all_nodes_ending_with_a(struct network *net, struct network *collected)
{
	collected->num = 0;
	collected->nodes = NULL;

	for (size_t i = 0; i < net->num; i++) {
		struct node *n = &net->nodes[i];
		if (n->node[2] == 'A') {
			network_append_node(collected, n);
		}
	}
}

void network_step_in_all(struct network *net, char dir)
{
	for (size_t i = 0; i < net->num; i++) {
		struct node *node = &net->nodes[i];
		net->nodes[i] = *node_step(node, dir);
	}
}

bool network_all_nodes_reached_z(struct network *net)
{
	for (size_t i = 0; i < net->num; i++) {
		struct node *n = &net->nodes[i];

		if (n->node[2] != 'Z') {
			return false;
		}
	}

	return true;
}

/* Find all nodes ending with with A and add them to a list.
 * Implement a step function that does the same step on all nodes in the lists.
 * Implement a verify function that checks each node in the list if it ends with an Z.
 *
 * This will take forever, see below...
 */
long long solve_second_part_bad(struct network *net, char *instructions)
{
	long long steps = 0;
	char *cur_inst = instructions;

	struct network cur_nodes;
	network_collect_all_nodes_ending_with_a(net, &cur_nodes);

	// print_network(&cur_nodes);

	while (1) {
		network_step_in_all(&cur_nodes, *cur_inst++);
		steps++;

		if (network_all_nodes_reached_z(&cur_nodes)) {
			return steps;
		}
		/* Did we reach the end of instructions? Then we start from the beginning. */
		if (*cur_inst == '\n') {
			// printf("Reseting\n");
			cur_inst = instructions;
		}
	}
}

long long gcd_func(long long a, long long b)
{
	if (b == 0)
		return a;
	return gcd_func(b, a % b);
}

/* Alternative solution for the second part, since my first solution for it doesn't seem finish
 *
 *
 * Find all nodes ending with with A and add them to a list. It looks like there are 6 of them.
 *
 * For each node run the instruction steps and continue until you get to the node ending with Z.
 * Mark the number of steps and the exact label and continoue until you gathered all six of them.
 * (This might not be possible for all nodes starting with A, resolve this later)
 *
 * You should end up with six lists of step-label pairs, one list for each starting node.
 *
 * Each list should have some pair which number of steps is found in all lists.
 *
 * So, after I implemented the above code and and I ran it, I learned something: each node ending
 * with A has its own node that ends with Z, which is not shared by any other node that ends with A.
 * Also the number of steps that are needed to go from Z to the same Z (for the same starting node
 * A) is always constant.
 *
 * If I collect all the steps that takes each node A to reach its node Z and find a smallest common
 * multiple between the steps, I will find my answer.
 */
long long solve_second_part(struct network *net, char *instructions)
{

	struct network cur_nodes;
	network_collect_all_nodes_ending_with_a(net, &cur_nodes);

	// print_network(&cur_nodes);

	long long all_steps[6];

	for (size_t i = 0; i < cur_nodes.num; i++) {
		struct node *cur = &cur_nodes.nodes[i];

		int num_z_found = 0;
		char *cur_inst = instructions;
		long long steps = 0;

		while (1) {
			cur = node_step(cur, *cur_inst++);
			steps++;

			/* Did we reach the goal? Then we stop stepping. */
			if (cur->node[2] == 'Z') {
				// printf("Reached: %s, %lli: steps\n", cur->node, steps);
				all_steps[i] = steps;
				break;
			}

			if (*cur_inst == '\n') {
				// printf("Reseting\n");
				cur_inst = instructions;
			}
		}
	}
	long long lcm = all_steps[0];

	long long gcd = all_steps[0];

	// Loop through the array and find GCD
	// use GCD to find the LCM
	for (int i = 1; i < 6; i++) {
		gcd = gcd_func(all_steps[i], lcm);
		lcm = (lcm * all_steps[i]) / gcd;
	}

	return lcm;
}

int main()
{
	FILE *f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	char *instructions = NULL;
	char *line;
	size_t num_char;

	getline(&instructions, &num_char, f);

	/* Throw away empty line */
	getline(&line, &num_char, f);

	struct network net;
	parse_network_from_file(f, &net);

	printf("%s\n", instructions);

	// print_network(&net);
	network_connect(&net);

	printf("Part 1: %i\n", solve_first_part(&net, instructions));
	// printf("Part 2: %lli\n", solve_second_part_bad(&net, instructions));
	printf("Part 2: %lli\n", solve_second_part(&net, instructions));
}
