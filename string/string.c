#include "../string/string.h"

int str_length(const char *s)
{
    int len = 0;

    if (s == (void *)0)
        return 0;
    while (s[len] != '\0')
        len++;
    return len;
}

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
    return a[i] - b[i];
}

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

int str_split(const char *src, char delimiter, char tokens[][MAX_TOKEN_LENGTH],
              int max_tokens)
{
    int token_count = 0;
    int char_index = 0;
    int i = 0;

    if (src == (void *)0 || max_tokens <= 0)
        return 0;

    while (src[i] != '\0' && token_count < max_tokens)
    {
        if (src[i] == delimiter)
        {
            tokens[token_count][char_index] = '\0';
            token_count++;
            char_index = 0;
        }
        else
        {
            if (char_index < MAX_TOKEN_LENGTH - 1)
            {
                tokens[token_count][char_index] = src[i];
                char_index++;
            }
        }
        i++;
    }

    if (char_index > 0 || i > 0)
    {
        tokens[token_count][char_index] = '\0';
        token_count++;
    }

    return token_count;
}

void int_to_string(int value, char *buffer, int buf_size)
{
    int  i = 0;
    int  is_negative = 0;
    int  start;
    int  end;
    char temp;

    if (buffer == (void *)0 || buf_size <= 0)
        return;

    if (value == 0)
    {
        if (buf_size >= 2)
        {
            buffer[0] = '0';
            buffer[1] = '\0';
        }
        else
            buffer[0] = '\0';
        return;
    }

    if (value < 0)
    {
        is_negative = 1;
        value = -value;
    }

    while (value > 0 && i < buf_size - 1)
    {
        buffer[i] = '0' + (value % 10);
        value = value / 10;
        i++;
    }

    if (is_negative && i < buf_size - 1)
    {
        buffer[i] = '-';
        i++;
    }

    buffer[i] = '\0';

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

int string_to_int(const char *s)
{
    int result = 0;
    int sign = 1;
    int i = 0;

    if (s == (void *)0)
        return 0;

    while (s[i] == ' ' || s[i] == '\t' || s[i] == '\n')
        i++;

    if (s[i] == '-')
    {
        sign = -1;
        i++;
    }
    else if (s[i] == '+')
        i++;

    while (s[i] >= '0' && s[i] <= '9')
    {
        result = result * 10 + (s[i] - '0');
        i++;
    }

    return result * sign;
}
