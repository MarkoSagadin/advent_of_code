/** @file file_utility.h
 *
 * @brief Interface file for utility functions that operate on the FILE stream.
 */

#ifndef FILE_UTILITY_H
#define FILE_UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

/* Returns number of lines in the given file. */
int get_num_lines(FILE *f);

/* Stops at newline */
int get_num_chars_in_line(FILE *f);

/* Ignore newline characters */
void copy_file_into_array(FILE *f, char *array);

int get_num_tokens_in_line(char *line, int delim);

/* Moves to the start of the nth line in the given file. */
void move_to_nth_line(FILE *f, int nth_line);

int get_num_chars_in_nth_line(FILE *f, int nth_line);

/* Fills given line with the string found in the nth line in the given FILE f.
 * Max line length indicates the maximum number of characters that line can
 * hold (including null terminator).
 * Returns null if read error occured, otherwise it returns pointer to line. */
char *get_nth_line(FILE *f, char *line, int max_line_len, int nth_line);

/* Returns number of characters of space delimited, words in given string line.
 * It starts counting from starting word index, up to, but excluding the
 * end word index.
 *
 * Example:
 *  input line:     "Hello I am a string\n"
 *  function call:  get_len_sub_words(f, 1, 3)
 *  output:         4
 */
int get_len_sub_words(char *line, int start, int end);

/* Fills given words string with space delimited words that are found in given
 * string line. It starts filling from starting word index, up to, but excluding
 * the end word index. It also adds a null terminator at the end of the filled
 * string.
 *
 * It is the responsibility of the caller to allocate enough memory for words
 * string.
 *
 * Example:
 *  current line:   "Hello I am a string\n"
 *  function call:  get_sub_words(f, 1, 3)
 *  output:         "I am\0"
 * */
void get_sub_words(char *line, char *sub_words, int start, int end);

/* Returns dynamically alocated sub_word in similiar sense as above function,
 * but is performed on the whole given FILE.
 * Do not forget to free after use.
 * If string could not be allocated NULL is returned. */
char *get_sub_words_in_nth_line(FILE *f, int nth_line, int start, int end);

#ifdef __cplusplus
}
#endif

#endif /* FILE_UTILITY_H */
/*** end of file ***/
