#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* What I need to implement:
 * Parsing function.
 * Function for determining the hand type.
 * Function for determining the "second ordering rule".
 * qsort callback that uses above two functions to sort hands.
 * For loop that determines the total winnings, the requested puzzle answer.
 * */

char card_order[] = {'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2'};
char card_order_with_joker[] = {'A', 'K', 'Q', 'T', '9', '8', '7', '6', '5', '4', '3', '2', 'J'};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

struct hand {
	char cards[5];
	long bid;
};

struct hands {
	struct hand *hands;
	size_t num;
};

void hands_append(struct hands *hands, struct hand *hand)
{
	struct hands *h = hands;
	/* Create space  */
	h->hands = realloc(h->hands, (h->num + 1) * sizeof(struct hand));
	/* Copy contents into new space */
	h->hands[h->num++] = *hand;
}

void parse_hand_from_line(char *line, struct hand *hand)
{
	char *h = strtok(line, " ");
	long bid = atoi(strtok(NULL, " "));

	strcpy(hand->cards, h);
	hand->bid = bid;
}

void parse_hands_from_file(FILE *f, struct hands *hands)
{
	hands->hands = NULL;
	hands->num = 0;

	while (1) {
		char *line = NULL;
		size_t num_char;
		if (getline(&line, &num_char, f) == -1) {
			break;
		}

		struct hand hand;
		parse_hand_from_line(line, &hand);

		hands_append(hands, &hand);
	}
}

void print_hands(struct hands *hands)
{
	for (size_t i = 0; i < hands->num; i++) {
		struct hand *h = &hands->hands[i];
		printf("hand[%lu]: cards=%s, bid=%i\n", i, h->cards, h->bid);
	}
}

int calc_hand_type_score(const struct hand *hand, bool use_joker)
{
	/* To make later processing easier, just make an array where indexes represent characters
	 * and values the number of occurances in the hand. */
	int card_count[256];
	memset(card_count, 0, sizeof(card_count));

	int num_jokers = 0;

	for (size_t i = 0; i < strlen(hand->cards); i++) {
		char c = hand->cards[i];

		if (use_joker && c == 'J') {
			num_jokers++;
		} else {
			card_count[c]++;
		}
	}

	/* Find the max two occurances of the cards. */
	int max1 = 0;
	int max2 = 0;

	for (size_t i = 0; i < 256; i++) {
		if (card_count[i] > max1) {
			max2 = max1;
			max1 = card_count[i];
		} else if (card_count[i] > max2) {
			max2 = card_count[i];
		}
	}

	/* Hand Type determining logic */
	/* Five of a kind */
	if (max1 == 5) {
		return 6;
	}

	/* Four of a kind */
	if (max1 == 4) {
		return use_joker ? 5 + num_jokers : 5;
	}

	/* Full house */
	if (max1 == 3 && max2 == 2) {
		/* No jokers here */
		return 4;
	}

	/* Three of a kind */
	if (max1 == 3) {
		/* Little weird, but basically three of a kind plus joker make four of a kind, so
		 * return 5 instead of 3. */
		return use_joker ? ((num_jokers == 2) ? 6 : 3 + num_jokers * 2) : 3;
	}

	/* Two pairs */
	if (max1 == 2 && max2 == 2) {
		/* Again, little weird, but two pairs with a joker can only make a full house */
		return use_joker ? 2 + num_jokers * 2 : 2;
	}

	/* One pair */
	if (max1 == 2) {
		/* Convenient way to describe the score. */
		return use_joker ? ((num_jokers == 3) ? 6 : 1 + num_jokers * 2) : 1;
	}

	/* High card or jokers */
	if (!use_joker) {
		return 0;
	}

	if (num_jokers == 0) {
		return 0;
	}

	if (num_jokers >= 4) {
		return 6;
	}

	return num_jokers * 2 - 1;
}

int get_card_order_score(char c, bool use_joker)
{
	char *order = use_joker ? card_order_with_joker : card_order;

	for (size_t i = 0; i < ARRAY_SIZE(card_order); i++) {
		if (c == order[i]) {
			return i;
		}
	}

	printf("This shouldn't happen");
	exit(0);
	return 0;
}

int calc_second_order_comparison(const struct hand *hand1, const struct hand *hand2, bool use_joker)
{
	for (size_t i = 0; i < 5; i++) {
		int score1 = get_card_order_score(hand1->cards[i], use_joker);
		int score2 = get_card_order_score(hand2->cards[i], use_joker);

		/* Smaller score is actually larger. */
		if (score1 > score2) {
			return -1;
		}
		if (score1 < score2) {
			return 1;
		}
	}

	printf("This shouldn't happen");
	exit(0);
	return 0;
}

int compare_hands_general(const struct hand *hand1, const struct hand *hand2, bool use_joker)
{
	/* Return -1 if h1 < h2, 0 if h1 == h2, 1 if h1 > h2 */
	int score1 = calc_hand_type_score(hand1, use_joker);
	int score2 = calc_hand_type_score(hand2, use_joker);

	if (score1 < score2) {
		return -1;
	}

	if (score1 > score2) {
		return 1;
	}

	return calc_second_order_comparison(hand1, hand2, use_joker);
}

int compare_hands_cb(const void *hand1, const void *hand2)
{
	return compare_hands_general(hand1, hand2, false);
}

int compare_hands_using_joker_cb(const void *hand1, const void *hand2)
{
	return compare_hands_general(hand1, hand2, true);
}

int main()
{
	FILE *f = fopen("src/input.txt", "r");
	if (f == NULL) {
		printf("No file found!\n");
		return 0;
	}

	struct hands hands;

	parse_hands_from_file(f, &hands);

	/* This will sort them in the ascending order. */
	qsort(hands.hands, hands.num, sizeof(struct hand), compare_hands_cb);
	// print_hands(&hands);

	long rank = 1;
	long total_winnings = 0;

	for (size_t i = 0; i < hands.num; i++) {
		// printf("hand[%lu]: cards=%s, bid=%03i, rank=%i\n", i, hands.hands[i].cards,
		//        hands.hands[i].bid, rank);
		total_winnings += hands.hands[i].bid * rank++;
	}

	printf("Part 1 is: %li\n", total_winnings);

	qsort(hands.hands, hands.num, sizeof(struct hand), compare_hands_using_joker_cb);

	rank = 1;
	total_winnings = 0;

	for (size_t i = 0; i < hands.num; i++) {
		// printf("hand[%lu]: cards=%s, bid=%03i, rank=%i\n", i, hands.hands[i].cards,
		//        hands.hands[i].bid, rank);
		total_winnings += hands.hands[i].bid * rank++;
	}
	printf("Part 2 is: %li\n", total_winnings);
}
