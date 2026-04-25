#ifndef CUSTOM_STRING_H
#define CUSTOM_STRING_H

#define MAX_TOKEN_LENGTH 64

int str_length(const char *s);
int str_compare(const char *a, const char *b);
void str_copy(char *dest, const char *src);
void str_ncopy(char *dest, const char *src, int max_len);
void str_concat(char *dest, const char *src);
int str_find_char(const char *s, char c);
int str_split(const char *src, char delimiter, char tokens[][MAX_TOKEN_LENGTH],
              int max_tokens);
void int_to_string(int value, char *buffer, int buf_size);
int string_to_int(const char *s);

#endif
