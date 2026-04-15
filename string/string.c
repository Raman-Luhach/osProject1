/* ========================================================================== */
/*                         CUSTOM STRING LIBRARY                             */
/*                                                                           */
/*  Implementation of string manipulation and conversion functions.          */
/*  No <string.h> is used anywhere in this file.                            */
/* ========================================================================== */

#include "../string/string.h"

/* -------------------------------------------------------------------------- */
/*  str_length                                                               */
/*  Walks the string until the null terminator and counts each character.    */
/* -------------------------------------------------------------------------- */
int str_length(const char *s)
{
    int len = 0;

    if (s == (void *)0)
        return 0;
    while (s[len] != '\0')
        len++;
    return len;
}

/* -------------------------------------------------------------------------- */
/*  str_compare                                                              */
/*  Compares two strings character by character.                             */
/*  Returns the difference at the first mismatch, or 0 if identical.        */
/* -------------------------------------------------------------------------- */
int str_compare(const char *a, const char *b)
{
    int i = 0;

    if (a == (void *)0 || b == (void *)0)
        return -1;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
            return a[i] - b[i];
        i++;
    }
    /* If one string is longer, the difference of terminators will be nonzero */
    return a[i] - b[i];
}

/* -------------------------------------------------------------------------- */
/*  str_copy                                                                 */
/*  Copies every character from src to dest, including the null terminator.  */
/* -------------------------------------------------------------------------- */
void str_copy(char *dest, const char *src)
{
    int i = 0;

    if (dest == (void *)0 || src == (void *)0)
        return;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

/* -------------------------------------------------------------------------- */
/*  str_ncopy                                                                */
/*  Copies at most (max_len - 1) characters and always null-terminates.     */
/* -------------------------------------------------------------------------- */
void str_ncopy(char *dest, const char *src, int max_len)
{
    int i = 0;

    if (dest == (void *)0 || src == (void *)0 || max_len <= 0)
        return;
    while (i < max_len - 1 && src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

/* -------------------------------------------------------------------------- */
/*  str_concat                                                               */
/*  Appends the src string to the end of dest.                               */
/* -------------------------------------------------------------------------- */
void str_concat(char *dest, const char *src)
{
    int dest_len;
    int i;

    if (dest == (void *)0 || src == (void *)0)
        return;
    dest_len = str_length(dest);
    i = 0;
    while (src[i] != '\0')
    {
        dest[dest_len + i] = src[i];
        i++;
    }
    dest[dest_len + i] = '\0';
}

/* -------------------------------------------------------------------------- */
/*  str_find_char                                                            */
/*  Returns the index of the first occurrence of c, or -1 if not found.     */
/* -------------------------------------------------------------------------- */
int str_find_char(const char *s, char c)
{
    int i = 0;

    if (s == (void *)0)
        return -1;
    while (s[i] != '\0')
    {
        if (s[i] == c)
            return i;
        i++;
    }
    return -1;
}

/* -------------------------------------------------------------------------- */
/*  str_split                                                                */
/*  Splits a string by the given delimiter character.                        */
/*  Stores each token in tokens[][MAX_TOKEN_LENGTH].                        */
/*  Returns the number of tokens extracted.                                  */
/* -------------------------------------------------------------------------- */
int str_split(const char *src, char delimiter, char tokens[][MAX_TOKEN_LENGTH],
              int max_tokens)
{
    int token_count = 0;   /* how many tokens we have found so far  */
    int char_index  = 0;   /* position within the current token     */
    int i           = 0;   /* position within the source string     */

    if (src == (void *)0 || max_tokens <= 0)
        return 0;

    while (src[i] != '\0' && token_count < max_tokens)
    {
        if (src[i] == delimiter)
        {
            /* End the current token and move to the next slot */
            tokens[token_count][char_index] = '\0';
            token_count++;
            char_index = 0;
        }
        else
        {
            /* Add the character to the current token (bounds check) */
            if (char_index < MAX_TOKEN_LENGTH - 1)
            {
                tokens[token_count][char_index] = src[i];
                char_index++;
            }
        }
        i++;
    }

    /* Terminate and count the last token (if there was any content) */
    if (char_index > 0 || i > 0)
    {
        tokens[token_count][char_index] = '\0';
        token_count++;
    }

    return token_count;
}

/* -------------------------------------------------------------------------- */
/*  int_to_string                                                            */
/*  Converts an integer to its decimal string representation.                */
/*                                                                           */
/*  Algorithm:                                                               */
/*    1. Handle the special case of 0.                                       */
/*    2. If negative, record the sign and work with the positive value.      */
/*    3. Extract digits from least-significant to most-significant.          */
/*    4. Reverse the digit portion in place.                                 */
/* -------------------------------------------------------------------------- */
void int_to_string(int value, char *buffer, int buf_size)
{
    int  i = 0;
    int  is_negative = 0;
    int  start;
    int  end;
    char temp;

    if (buffer == (void *)0 || buf_size <= 0)
        return;

    /* Special case: zero */
    if (value == 0)
    {
        if (buf_size >= 2)
        {
            buffer[0] = '0';
            buffer[1] = '\0';
        }
        else
        {
            buffer[0] = '\0';
        }
        return;
    }

    /* Handle negative numbers */
    if (value < 0)
    {
        is_negative = 1;
        value = -value;
    }

    /* Extract digits in reverse order */
    while (value > 0 && i < buf_size - 1)
    {
        buffer[i] = '0' + (value % 10);
        value = value / 10;
        i++;
    }

    /* Add the minus sign if negative */
    if (is_negative && i < buf_size - 1)
    {
        buffer[i] = '-';
        i++;
    }

    buffer[i] = '\0';

    /* Reverse the entire string in place */
    start = 0;
    end = i - 1;
    while (start < end)
    {
        temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

/* -------------------------------------------------------------------------- */
/*  string_to_int                                                            */
/*  Parses an integer from the beginning of a string.                        */
/*  Handles optional leading '-' sign. Stops at first non-digit character.   */
/* -------------------------------------------------------------------------- */
int string_to_int(const char *s)
{
    int result = 0;
    int sign   = 1;
    int i      = 0;

    if (s == (void *)0)
        return 0;

    /* Skip leading whitespace */
    while (s[i] == ' ' || s[i] == '\t' || s[i] == '\n')
        i++;

    /* Handle optional sign */
    if (s[i] == '-')
    {
        sign = -1;
        i++;
    }
    else if (s[i] == '+')
    {
        i++;
    }

    /* Accumulate digits */
    while (s[i] >= '0' && s[i] <= '9')
    {
        result = result * 10 + (s[i] - '0');
        i++;
    }

    return result * sign;
}
