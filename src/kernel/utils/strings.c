#ifndef STRINGS
#define STRINGS

int string_len(char* s) {
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

#endif