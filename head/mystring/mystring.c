#include "mystring.h"

#include <stdio.h>
#include <string.h>
void intochar(char *s, int a) {
    int len = 0;
    if (a == 0) {
        s[0] = 48;
        s[1] = '\0';
        return;
    }
    while (a > 0) {
        s[len] = a % 10 + 48;
        a /= 10;
        ++len;
    }
    s[len] = '\0';
    for (int i = 0; i < len / 2; ++i) {
        char t = s[i];
        s[i] = s[len - i - 1];
        s[len - i - 1] = t;
    }
}

void chartoint(char *s, int *a) {
    int len = strlen(s);
    int u = 0;
    for (int i = 0; i < len; ++i) {
        if(s[i] < 48 || s[i] > 57) {
            *a = -1;
            return;
        }
        u = u * 10 + s[i] - 48;
    }
    *a = u;
}

void comb(char* a, char* b, char* c){
    int lenb = strlen(b);
    int lenc = strlen(c);
    for(int i = 0; i < lenb; ++i){
        a[i] = b[i];
    }
    for(int i = 0; i < lenc; ++i){
        a[lenb + i] = c[i];
    }
    a[lenb + lenc] = '\0';
}