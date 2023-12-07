#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct map {
	long long src;
	long long dst;
	long long range;
};

struct category_map {
	struct map *maps;
	size_t num_maps;
};

#define move_until(line, delim)                                                                    \
	do {                                                                                       \
		while (*(line)++ != (delim)) {                                                     \
		}                                                                                  \
	} while (0)

static void fill_array(char *line, long long *numbers, size_t *num)
{
	*num = 0;
	while (1) {
		char *next;
		numbers[(*num)++] = strtoll(line, &next, 10);
		next++;

		if (*next == '\0') {
			return;
		}
		line = next;
	}
}

#define print_array(msg, arr, num)                                                                 \
	do {                                                                                       \
		printf("%s", msg);                                                                 \
		for (int i = 0; i < num; i++) {                                                    \
			printf("%lli ", arr[i]);                                                   \
		}                                                                                  \
		printf("\n");                                                                      \
	} while (0)

/* Append category map to the list of the category maps */
void category_map_append(struct category_map **cat_maps, size_t *num_cat_maps,
			 struct category_map *cat_map)
{
	/* Create space  */
	*cat_maps = realloc(*cat_maps, (*num_cat_maps + 1) * sizeof(struct category_map));

	/* Copy contents into new space */
	(*cat_maps)[*num_cat_maps] = *cat_map;

	*num_cat_maps += 1;
}

/* Append map to the list inside category maps */
void map_append(struct map **maps, size_t *num_maps, struct map *map)
{
	/* Create space  */
	*maps = realloc(*maps, (*num_maps + 1) * sizeof(struct map));

	/* Copy contents into new space */
	(*maps)[*num_maps] = *map;
	*num_maps += 1;
}

bool fill_maps(FILE *f, struct map **maps, size_t *num_maps)
{
	char *line = malloc(128);
	size_t num_char;

	*maps = NULL;
	*num_maps = 0;

	while (1) {
		if (getline(&line, &num_char, f) == -1) {
			return true;
		}

		if (line[0] == '\n') {
			return false;
		}

		/* Drop lines until you get to the start of the map numbers. */
		if (!isdigit(line[0])) {
			continue;
		}

		long long numbers[3];
		size_t num;
		fill_array(line, numbers, &num);

		struct map map = {
			.src = numbers[1],
			.dst = numbers[0],
			.range = numbers[2],
		};

		map_append(maps, num_maps, &map);
	}
}

void fill_category_map(FILE *f, struct category_map **cat_maps, size_t *num_cat_maps)
{
	*cat_maps = NULL;
	*num_cat_maps = 0;

	while (1) {
		struct category_map cat_map = {0};

		bool done = fill_maps(f, &cat_map.maps, &cat_map.num_maps);
		category_map_append(cat_maps, num_cat_maps, &cat_map);
		if (done) {
			break;
		}
	}
}

bool seed_apply_map(long long *seed, struct map *map)
{
	if (map->src <= *seed && *seed < map->src + map->range) {
		*seed = map->dst + *seed - map->src;
		return true;
	}
	return false;
}

void print_category_maps(struct category_map *cat_maps, size_t num_cat_maps)
{
	for (int i = 0; i < num_cat_maps; i++) {
		struct category_map *cat_m = &cat_maps[i];
		printf("cat_map[%i]:\n", i);
		for (int j = 0; j < cat_m->num_maps; j++) {
			struct map *m = &cat_m->maps[j];
			printf("map[%i]: src=%lli, dst=%lli, range=%lli\n", j, m->src, m->dst,
			       m->range);
		}
	}
}

void seed2_append(long long **seed2, size_t *num_seeds2, long long seed)
{
	/* Create space  */
	*seed2 = realloc(*seed2, (*num_seeds2 + 1) * sizeof(long long));

	/* Copy contents into new space */
	(*seed2)[*num_seeds2] = seed;
	*num_seeds2 += 1;
}

void create_seeds2(long long **seeds2, size_t *num_seeds2, long long *seeds, size_t num_seeds)
{
	for (size_t i = 0; i < num_seeds; i = i + 2) {
		long long start = seeds[i];
		long long range = seeds[i + 1];
		for (size_t j = start; j < start + range; j++) {
			seed2_append(seeds2, num_seeds2, j);
		}
	}
}

long long pass_seed_through_maps(struct category_map *cat_maps, size_t num_cat_maps, long long seed)
{
	for (size_t j = 0; j < num_cat_maps; j++) {
		struct category_map *cat_map = &cat_maps[j];

		// printf("cat_map[%zu]:\n", j);

		for (size_t k = 0; k < cat_map->num_maps; k++) {
			struct map map = cat_map->maps[k];
			/* Try each map to the seed, as soon as one succeds you can
			 * stop doing that, and go to the next category_map. */
			if (seed_apply_map(&seed, &map)) {
				break;
			}
		}
	}

	return seed;
}

struct large_seed {
	long long start;
	long long range;
	bool map_applied;
	bool removed;
};

void lseed_append(struct large_seed **lseeds, size_t *num_lseeds, struct large_seed *lseed)
{
	/* Create space  */
	*lseeds = realloc(*lseeds, (*num_lseeds + 1) * sizeof(struct large_seed));

	/* Copy contents into new space */
	(*lseeds)[*num_lseeds] = *lseed;
	*num_lseeds += 1;
}

struct large_seed lseed_create(long long start, long long range)
{
	return (struct large_seed){
		.start = start,
		.range = range,
		.map_applied = false,
		.removed = false,
	};
}

void large_seed_apply_map(struct large_seed **lseeds, size_t *num_lseeds, struct map *map,
			  struct large_seed *lseed)
{
	long long lseed_end = lseed->start + lseed->range - 1;
	long long map_end = map->src + map->range - 1;

	/* Is large seed inside the map? Then just modify start. */
	// < | | >
	if (map->src <= lseed->start && lseed_end <= map_end) {
		lseed->start = map->dst + lseed->start - map->src;
		lseed->map_applied = true;
		return;
	}

	/* Is large seed completly out of the map? Do not do anything with it. */
	//  | | < > or < > | |
	if (lseed_end < map->src || map_end < lseed->start) {
		return;
	}

	/* Is large seed overlapping with the map on the left side? */
	// | < | >
	if (lseed->start < map->src && map->src <= lseed_end) {

		/* Create two new seeds */
		struct large_seed out_lseed = lseed_create(lseed->start, map->src - lseed->start);
		struct large_seed in_lseed =
			lseed_create(map->src, lseed->start + lseed->range - map->src);
		in_lseed.start = map->dst + in_lseed.start - map->src;

		in_lseed.map_applied = true;

		/* append them */
		lseed_append(lseeds, num_lseeds, &in_lseed);
		lseed_append(lseeds, num_lseeds, &out_lseed);

		/* and remove old one since it was split. */
		lseed->removed = true;
		return;
	}

	/* Is large seed overlapping with the map on the right side? */
	// < | > |
	if (lseed->start <= map_end && map_end < lseed_end) {
		/* Create two new seeds */
		struct large_seed in_lseed =
			lseed_create(lseed->start, map->src + map->range - lseed->start);
		struct large_seed out_lseed =
			lseed_create(map->src + map->range,
				     lseed->start + lseed->range - (map->src + map->range));
		in_lseed.start = map->dst + in_lseed.start - map->src;
		in_lseed.map_applied = true;
		/* append them */
		lseed_append(lseeds, num_lseeds, &in_lseed);
		lseed_append(lseeds, num_lseeds, &out_lseed);
		struct large_seed *l = &in_lseed;

		/* and remove old one since it was split. */
		lseed->removed = true;

		return;
	}
}
void print_lseeds(struct large_seed *lseeds, size_t num)
{
	for (size_t i = 0; i < num; i++) {
		struct large_seed *l = &lseeds[i];
		printf("lseed[%zu]: start=%lli, range=%lli, map_applied=%u, removed=%u\n", i,
		       l->start, l->range, l->map_applied, l->removed);
	}
}

long long pass_large_seed_through_maps(struct category_map *cat_maps, size_t num_cat_maps,
				       long long start, long long range)
{
	/* Create first large seed and add it to the list of large seeds. */
	struct large_seed lseed = lseed_create(start, range);
	struct large_seed *lseeds = NULL;
	size_t num_lseeds = 0;
	lseed_append(&lseeds, &num_lseeds, &lseed);

	for (size_t i = 0; i < num_cat_maps; i++) {
		struct category_map *cat_map = &cat_maps[i];

		// print_lseeds(lseeds, num_lseeds);

		for (size_t j = 0; j < cat_map->num_maps; j++) {
			struct map map = cat_map->maps[j];

			for (size_t k = 0; k < num_lseeds; k++) {
				struct large_seed *lseed = &lseeds[k];

				/* Skip removed lseeds, they were removed in large_seed_apply_map.*/
				if (lseed->removed || lseed->map_applied) {
					continue;
				}
				/* Split, creation of new lseeds and removal of the previous seed
				 * should be done inside of this function.
				 * */
				large_seed_apply_map(&lseeds, &num_lseeds, &map, lseed);
			}
		}
		/* Reset all maps_applied flags at the end of interating through category map, so
		 * that in the next category map they are checked again. */
		for (size_t k = 0; k < num_lseeds; k++) {
			lseeds[k].map_applied = false;
		}
	}

	/* Find minimum */
	long long min = LLONG_MAX;

	for (size_t i = 0; i < num_lseeds; i++) {
		if (lseeds[i].start < min && !lseeds[i].removed) {
			min = lseeds[i].start;
		}
	}

	return min;
}

int main()
{
	FILE *f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	char *line = malloc(128);
	size_t num_char;

	long long seeds[32];
	size_t num_seeds;

	struct category_map *cat_maps = NULL;
	size_t num_cat_maps = 0;

	getline(&line, &num_char, f);
	move_until(line, ':');
	fill_array(line, seeds, &num_seeds);

	print_array("seeds: ", seeds, num_seeds);

	/* One read to throw away the line, to make fill_category_map logic easier. */
	getline(&line, &num_char, f);
	fill_category_map(f, &cat_maps, &num_cat_maps);

	// print_category_maps(cat_maps, num_cat_maps);

	long long min1 = LLONG_MAX;
	long long min2 = LLONG_MAX;

	/* First part */
	for (size_t i = 0; i < num_seeds; i++) {
		long long seed = seeds[i];
		seed = pass_seed_through_maps(cat_maps, num_cat_maps, seed);
		if (seed < min1) {
			min1 = seed;
		}
	}

	/* Second part */
	for (size_t i = 0; i < num_seeds; i = i + 2) {
		long long start = seeds[i];
		long long range = seeds[i + 1];

		long long min = pass_large_seed_through_maps(cat_maps, num_cat_maps, start, range);
		// printf("location seed[%zu]: %lli\n", i, seed);
		if (min < min2) {
			min2 = min;
		}
	}

	printf("Part 1 is: %llu\n", min1);
	printf("Part 2 is: %llu\n", min2);
}
