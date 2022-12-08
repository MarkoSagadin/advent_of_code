#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Structure that can represent both a file or a dir. */
struct dirfile {
	/* If a file, then this is the size of the file, if a dir then the size is the sum of the
	 * sizes of the files it contains, directly or indirectly. */
	size_t size;
	/* Name of a file or dir. */
	char *name;
	/* Parent dir, for '/' is NULL. */
	struct dirfile *parent;
	/* Only dir has an array of dirfiles which it contains, for file it is NULL. */
	struct dirfile *dirfiles;
	/* Number of contained num_dirfiles, for file it is 0. */
	size_t num_dirfiles;
};

/* Change dir from a current head directory */
void chdir(struct dirfile **head, char *name)
{
	if (strcmp(name, "..") == 0) {
		*head = (*head)->parent;
		return;
	}

	for (size_t i = 0; i < (*head)->num_dirfiles; i++) {
		if (strcmp((*head)->dirfiles[i].name, name) == 0) {
			*head = &(*head)->dirfiles[i];
			return;
		}
	}
	printf("Error, directory with name %s was not found in %s directory!\n", name,
	       (*head)->name);
}

/* Make a dir in current head directory */
void mkdir(struct dirfile *head, char *name)
{
	struct dirfile *ptr =
		reallocarray(head->dirfiles, sizeof(struct dirfile), ++head->num_dirfiles);

	if (!ptr) {
		printf("Realloc failed!!!\n");
	}
	head->dirfiles = ptr;

	struct dirfile *new = &head->dirfiles[head->num_dirfiles - 1];

	new->size = 0;

	new->name = malloc(strlen(name) + 1);
	strcpy(new->name, name);
	new->parent = head;
	new->dirfiles = NULL;
	new->num_dirfiles = 0;
}

/* Make a file in current head directory */
void mkfile(struct dirfile *head, char *size, char *name)
{
	mkdir(head, name);
	head->dirfiles[head->num_dirfiles - 1].size = atoi(size);
}

void print_level(size_t level)
{
	for (size_t i = 0; i < level * 2; i++) {
		printf(" ");
	}
}

size_t calculate_size(struct dirfile *head, size_t *answer1)
{
	/* Iterate through each dir/file in directory and calculate size for it. */
	for (size_t i = 0; i < head->num_dirfiles; i++) {
		if (head->dirfiles[i].dirfiles) {
			/* Get the size of a specific dir and add it to the current dir. */
			head->size += calculate_size(&head->dirfiles[i], answer1);
		} else {
			/* Get the size of a specific file and add it to the current dir. */
			head->size += head->dirfiles[i].size;
		}
	}

	if (head->size <= 100000) {
		*answer1 += head->size;
	}
	return head->size;
}

void find_dir_to_delete(struct dirfile *head, size_t space_needed, size_t *answer2)
{
	/* Iterate through each dir/file in directory and find the one that matches the condition.
	 */
	for (size_t i = 0; i < head->num_dirfiles; i++) {
		if (head->dirfiles[i].dirfiles) {
			/* Get the size of a specific dir and add it to the current dir. */
			find_dir_to_delete(&head->dirfiles[i], space_needed, answer2);
		}
	}

	/* Condition */
	if (head->size >= space_needed) {
		if (head->size < *answer2) {
			*answer2 = head->size;
		}
	}
}

void print_tree(struct dirfile *head, size_t level)
{
	if (level == 0) {
		print_level(level);
		printf("- %s (dir, size=%lu)\n", head->name, head->size);
	}
	level++;

	for (size_t i = 0; i < head->num_dirfiles; i++) {
		if (head->dirfiles[i].dirfiles) {
			print_level(level);
			printf("- %s (dir, size=%lu)\n", head->dirfiles[i].name,
			       head->dirfiles[i].size);
			print_tree(&head->dirfiles[i], level);
		} else {
			print_level(level);
			printf("- %s (file, size=%lu)\n", head->dirfiles[i].name,
			       head->dirfiles[i].size);
		}
	}
}

int main()
{
	/* Open file, file path is a bit weird due to the way the executable is run */
	FILE *f = fopen("src/input.txt", "r");
	if (!f) {
		printf("No file found!\n");
		return 0;
	}

	char *line = NULL;
	size_t char_read = 0;

	/* Da root */
	struct dirfile root = {
		.name = "/",
		/* Unknown at the start */
		.size = 0,
		.parent = NULL,
		.dirfiles = NULL,
		.num_dirfiles = 0,
	};

	/* Create a non-existing dir above root, just so we can bootstrap ourselfs. */
	struct dirfile start = {
		.name = "start",
		.dirfiles = &root,
		.num_dirfiles = 1,
	};
	struct dirfile *head = &start;

	printf("\n");
	while (getline(&line, &char_read, f) > 0) {
		// printf("current dir: \"%s\", line: %s", head->name, line);

		if (line[0] == '$') {
			/* command to execute */
			char *command;
			char *argument;
			/* 'm' means that sscanf alocates the memory for a string dynamically. */
			int argc = sscanf(line, "$ %ms %ms", &command, &argument);
			if (strcmp(command, "cd") == 0) {
				chdir(&head, argument);
			}
			free(command);
			free(argument);

		} else {
			/* output of ls command */
			char *first;
			char *second;
			int argc = sscanf(line, "%ms %ms", &first, &second);
			if (strcmp(first, "dir") == 0) {
				mkdir(head, second);
			} else {
				mkfile(head, first, second);
			}
			free(first);
			free(second);
		}
	}

	/* Calculate sizes inside the tree, attach the result to root dir for completness. */
	head = &root;
	size_t answer1 = 0;

	head->size = calculate_size(head, &answer1);

	/* Make answer2 equal to the total size, so the compare logic in find_dir_to_delete will
	 * work. */
	size_t answer2 = 70000000;
	size_t space_needed = 30000000 - (70000000 - head->size);
	find_dir_to_delete(head, space_needed, &answer2);
	// print_tree(head, 0);

	printf("Answer, part 1: %lu\n", answer1);
	printf("Answer, part 2: %lu\n", answer2);
}
