#ifndef CHAR_UTILS_H
#define CHAR_UTILS_H

#include <stdbool.h>

static inline int char_cmp(char c1, char c2) {
    return (unsigned char)c1 - (unsigned char)c2;
}

static inline bool char_is_upper(char c) {
    return (c >= 'A' && c <= 'Z');
}

static inline bool char_is_lower(char c) {
    return (c >= 'a' && c <= 'z');
}

static inline char char_to_lower(char c) {
    if (char_is_upper(c)) {
        return c + ('a' - 'A');
    }
    return c;
}

static inline char char_to_upper(char c) {
    if (char_is_lower(c)) {
        return c - ('a' - 'A');
    }
    return c;
}

static inline int char_casecmp(char c1, char c2) {
    unsigned char u1 = (unsigned char)char_to_lower(c1);
    unsigned char u2 = (unsigned char)char_to_lower(c2);
    return u1 - u2;
}

static inline int chars_len(const char c[256])
{
    int len = 0;

    for (int i = 0; c[i] != '\0'; i++)
    {
        len++;
    }

    return len;
}

static inline int chars_to_int(const char c[256])
{
    int processed_number = 0;

    for (int i = 0; c[i] != '\0'; i++)
    {
        if (c[i] >= '0' && c[i] <= '9')
        {
            processed_number = processed_number * 10 + (c[i] - '0');
        }
    }

    return processed_number;
}

static inline int chars_cmp(const char c1[256], const char c2[256])
{
    int len_c1 = chars_len(c1);
    int len_c2 = chars_len(c2);
    
    if (len_c1 != len_c2)
    {
        return 0;
    }

    for (int i = 0; c1[i] != '\0'; i++)
    {
        if (char_cmp(c1[i], c2[i]))
        {
            return 0; // Discrepancy found, strings are not equal
        }
    }

    return 1; // All characters matched
}

#endif