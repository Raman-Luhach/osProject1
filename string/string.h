/* ========================================================================== */
/*                         CUSTOM STRING LIBRARY                             */
/*                                                                           */
/*  Provides string manipulation and conversion functions.                   */
/*  Replaces <string.h> which is FORBIDDEN in this project.                 */
/* ========================================================================== */

#ifndef CUSTOM_STRING_H
#define CUSTOM_STRING_H

/* Maximum number of characters per token when splitting strings. */
#define MAX_TOKEN_LENGTH 64

/* ---- Length & Comparison ---- */

/* Returns the number of characters in the string (excluding '\0'). */
int str_length(const char *s);

/* Compares two strings character by character.                              */
/* Returns 0 if equal, negative if a < b, positive if a > b.               */
int str_compare(const char *a, const char *b);

/* ---- Copy & Concatenation ---- */

/* Copies the entire source string into dest (including '\0').              */
/* Caller must ensure dest has enough space.                                */
void str_copy(char *dest, const char *src);

/* Copies at most (max_len - 1) characters from src into dest.              */
/* Always null-terminates the result.                                        */
void str_ncopy(char *dest, const char *src, int max_len);

/* Appends src to the end of dest. Caller must ensure dest has space.       */
void str_concat(char *dest, const char *src);

/* ---- Search ---- */

/* Returns the index of the first occurrence of character c in s.           */
/* Returns -1 if the character is not found.                                */
int str_find_char(const char *s, char c);

/* ---- Tokenization ---- */

/* Splits src by the given delimiter into the tokens array.                  */
/* Each token can hold up to MAX_TOKEN_LENGTH - 1 characters.               */
/* Returns the number of tokens found (up to max_tokens).                   */
int str_split(const char *src, char delimiter, char tokens[][MAX_TOKEN_LENGTH],
              int max_tokens);

/* ---- Conversions ---- */

/* Converts an integer to its string representation in the given buffer.    */
/* buf_size is the total size of the buffer (including space for '\0').     */
void int_to_string(int value, char *buffer, int buf_size);

/* Converts a string of digits (with optional leading '-') to an integer.   */
/* Stops at the first non-digit character.                                   */
int string_to_int(const char *s);

#endif /* CUSTOM_STRING_H */
