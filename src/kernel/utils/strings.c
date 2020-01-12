#ifndef STRINGS
#define STRINGS



int string_len(char* s) {
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

int string_cmp(char *s1, char *s2) {
    int len = string_len(s1);
    for (int i = 0; i < len; i++) {
        if (s1[i] != s2[i] || s2[i] == '\0') {
            return 0;
        }
    }
    return 1;
}

void reverse(char *s) {
    int start = 0;
    int end = string_len(s) - 1;
    char buff;
    while (start < end) {
        buff = *(s + start);
        *(s+start) = *(s+end);
        *(s+end) = buff;
        start++;
        end--;
    }
}

char* itoa(int num, char* str, int base)
{
    int i = 0;
    int neg = 0;
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    if (num < 0 && base == 10)
    {
        neg = 1;
        num = -num;
    }
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
    if (neg)
        str[i++] = '-';
    str[i] = '\0';
    reverse(str);
    return str;
}

#endif