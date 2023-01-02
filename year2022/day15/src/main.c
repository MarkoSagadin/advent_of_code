#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct position {
	int x;
	int y;
};

struct sens_beac_pair {
	struct position sensor;
	struct position beacon;
	int distance;
};

int calc_distance(struct position *a, struct position *b)
{
	return abs(a->x - b->x) + abs(a->y - b->y);
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

	struct sens_beac_pair pairs[100];
	size_t num_pairs = 0;

	/* Get line */
	while (getline(&line, &char_read, f) > 0) {
		/* parse line into rock points */
		sscanf(line, "Sensor at x=%i, y=%i: closest beacon is at x=%i, y=%i",
		       &pairs[num_pairs].sensor.x, &pairs[num_pairs].sensor.y,
		       &pairs[num_pairs].beacon.x, &pairs[num_pairs].beacon.y);

		pairs[num_pairs].distance =
			calc_distance(&pairs[num_pairs].sensor, &pairs[num_pairs].beacon);

		num_pairs++;

		/* Set line to NULL so getline can malloc new line */
		line = NULL;
	}

	/* Determine the maximum needed size of the grid */
	int min_x = INT_MAX;
	int min_y = INT_MAX;
	int max_x = 0;
	int max_y = 0;

	for (size_t i = 0; i < num_pairs; i++) {
		struct position *positions[2] = {&pairs[i].sensor, &pairs[i].beacon};
		for (size_t j = 0; j < 2; j++) {
			if (min_x > positions[j]->x)
				min_x = positions[j]->x;
			if (min_y > positions[j]->y)
				min_y = positions[j]->y;
			if (max_x < positions[j]->x)
				max_x = positions[j]->x;
			if (max_y < positions[j]->y)
				max_y = positions[j]->y;
		}
	}

	// printf("min_x :>> %i\n", min_x);
	// printf("max_x :>> %i\n", max_x);
	// printf("min_y :>> %i\n", min_y);
	// printf("max_y :>> %i\n", max_y);
	// printf("Height :>> %i\n", (max_y - min_y));
	// printf("Width :>> %i\n", max_x - min_x);

	/* Part 1 search
	 * For each x on y=2000000:
	 *	For sensor/beacon pair:
	 *		calculate x point , sensor distance
	 *		If any of above distances is inside sensor/beacon distance (and the point is
	 *		not occupied with sensor or beacon) mark the point
	 *		with # and continue.
	 * */
	size_t num_non_beacon_places = 0;

	struct position point = {
		.y = 2000000,
	};
	for (int x = min_x - 100000; x < max_x + 1000000; x++) {
		for (size_t j = 0; j < num_pairs; j++) {
			point.x = x;
			int point_dist = calc_distance(&pairs[j].sensor, &point);
			if (point_dist <= pairs[j].distance &&
			    memcmp(&point, &pairs[j].sensor, sizeof(struct position)) != 0 &&
			    memcmp(&point, &pairs[j].beacon, sizeof(struct position)) != 0) {
				num_non_beacon_places++;
				break;
			}
		}
	}
	printf("Answer, part 1: %lu\n", num_non_beacon_places);

	/* Part 2 search */
	/* Run through entire search place of x and y and search for a point that is not in the
	 * range of the any of the sensor/beacon pairs. Just doing this would take quite a lot of
	 * time, so we implement jumping along x axis. This means: as soon as the point is inside
	 * the range calculate two possible points along x, with fixed y and sensor/beacon pair
	 * distance and jump to one of them in the next iteration. */
	int upper_limit = 4000000;
	for (int y = 0; y < upper_limit; y++) {
		for (int x = 0; x < upper_limit; x++) {
			point.x = x;
			point.y = y;

			bool found_distress_beacon = true;
			for (size_t j = 0; j < num_pairs; j++) {
				int point_dist = calc_distance(&pairs[j].sensor, &point);

				if (point_dist <= pairs[j].distance) {
					found_distress_beacon = false;
					/* Calculate left size that we will pretend that it can be
					 * positive or negative */
					int left_side = pairs[j].distance -
							abs(point.y - pairs[j].sensor.y);

					/* Try minus first */
					int next_x = -left_side + pairs[j].sensor.x;

					if (point.x < next_x) {
						x = next_x - 1;
					} else {
						/* Try plus second */
						next_x = left_side + pairs[j].sensor.x;
						if (point.x < next_x) {
							/* Inside range, calculate jump */
							x = next_x - 1;
						}
					}
					break;
				}
			}
			if (found_distress_beacon) {
				printf("Found distress beacon at x: %i, y: %i\n", point.x, point.y);
				printf("Answer, part 2: %lu\n",
				       ((uint64_t)point.x) * 4000000UL + point.y);

				return 0;
			}
		}
	}
}
