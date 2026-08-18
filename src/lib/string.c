#include "../include/string.h"

size_t strlen(const char* str) {
    size_t len = 0;
    while (str && str[len])
        len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    if (n == 0) return 0;
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char* strcpy(char* dest, const char* src) {
    if (!dest || !src) return dest;
    char* orig = dest;
    while ((*dest++ = *src++));
    return orig;
}

char* strncpy(char* dest, const char* src, size_t n) {
    if (!dest || !src || n == 0) return dest;
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

char* strcat(char* dest, const char* src) {
    if (!dest || !src) return dest;
    char* orig = dest;
    while (*dest)
        dest++;
    while ((*dest++ = *src++));
    return orig;
}

char* strchr(const char* s, int c) {
    if (!s) return NULL;
    while (*s) {
        if (*s == (char)c)
            return (char*)s;
        s++;
    }
    if (c == '\0')
        return (char*)s;
    return NULL;
}

void* memset(void* buf, int val, size_t count) {
    unsigned char* p = (unsigned char*)buf;
    while (count--)
        *p++ = (unsigned char)val;
    return buf;
}

void* memcpy(void* dest, const void* src, size_t count) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    while (count--)
        *d++ = *s++;
    return dest;
}

void* memmove(void* dest, const void* src, size_t count) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    if (d < s) {
        while (count--)
            *d++ = *s++;
    } else {
        d += count;
        s += count;
        while (count--)
            *--d = *--s;
    }
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    while (n--) {
        if (*p1 != *p2)
            return *p1 - *p2;
        p1++;
        p2++;
    }
    return 0;
}

static void reverse_str(char* str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

char* itoa(int val, char* str, int base) {
    int i = 0;
    bool is_negative = false;

    if (val == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    uint32_t uval;
    if (val < 0 && base == 10) {
        is_negative = true;
        uval = (uint32_t)(-(int64_t)val);
    } else {
        uval = (uint32_t)val;
    }

    while (uval != 0) {
        uint32_t rem = uval % base;
        str[i++] = (rem > 9) ? (char)((rem - 10) + 'a') : (char)(rem + '0');
        uval = uval / base;
    }

    if (is_negative)
        str[i++] = '-';

    str[i] = '\0';
    reverse_str(str, i);
    return str;
}

char* utoa(uint32_t val, char* str, int base) {
    int i = 0;

    if (val == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    while (val != 0) {
        uint32_t rem = val % base;
        str[i++] = (rem > 9) ? (char)((rem - 10) + 'a') : (char)(rem + '0');
        val = val / base;
    }

    str[i] = '\0';
    reverse_str(str, i);
    return str;
}

char* utoa64(uint64_t val, char* str, int base) {
    int i = 0;

    if (val == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    while (val != 0) {
        uint64_t rem = val % base;
        str[i++] = (rem > 9) ? (char)((rem - 10) + 'a') : (char)(rem + '0');
        val = val / base;
    }

    str[i] = '\0';
    reverse_str(str, i);
    return str;
}

int atoi(const char* str) {
    if (!str) return 0;
    int res = 0;
    int sign = 1;
    int i = 0;

    while (str[i] == ' ')
        i++;

    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }

    while (str[i] >= '0' && str[i] <= '9') {
        res = res * 10 + (str[i] - '0');
        i++;
    }

    return sign * res;
}
