#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INF 2147483647

int n;
int a;
int r;
int b[30][30];
int pre[30][30];
char s[30][21];
void floyd() {
    for(int k = 0; k < n; ++k) {
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n; ++j) {
                if(b[i][k] != INF && b[k][j] != INF && b[i][k] + b[k][j] < b[i][j]) {
                    pre[i][j] = k;
                    b[i][j] = b[i][k] + b[k][j];
                }
            }
        }
    }
}
void find(int nump, int numq) {
    printf("%s", s[nump]);
    if(nump == numq) {
        printf("\n");
        return;
    }
    int road[30];
    road[0] = numq;
    int site = 1;
    int temp = numq;
    while(pre[nump][temp] != temp) {
        road[site] = pre[nump][temp];
        temp = pre[nump][temp];
        ++site;
    }
    road[site] = nump;
    for(int i = site - 1; i >= 0; --i) {
        printf("->(%d)->%s", b[road[i + 1]][road[i]], s[road[i]]);
    }
    printf("\n");
}
int main() {
    for(int i = 0; i < 30; ++i) {
        for(int j = 0; j < 30; ++j) {
            b[i][j] = INF;
            pre[i][j] = j;
        }
    }
    scanf("%d", &n);
    for(int i = 0; i < n; ++i) {
        scanf("%s", s[i]);
    }
    scanf("%d", &a);
    for(int i = 0; i < a; ++i) {
        char p[21];
        char q[21];
        int len;
        scanf("%s %s %d", p, q, &len);
        int nump;
        int numq;
        for(int j = 0; j < n; ++j) {
            if(strcmp(s[j], p) == 0) {
                nump = j;
                break;
            }
        }
        for(int j = 0; j < n; ++j) {
            if(strcmp(s[j], q) == 0) {
                numq = j;
                break;
            }
        }
        b[nump][numq] = len;
        b[numq][nump] = len;
    }
    floyd();
    scanf("%d", &r);
    for(int k = 0; k < r; ++k) {
        char p[21];
        char q[21];
        scanf("%s %s", p, q);
        int nump;
        int numq;
        for(int j = 0; j < n; ++j) {
            if(strcmp(s[j], p) == 0) {
                nump = j;
                break;
            }
        }
        for(int j = 0; j < n; ++j) {
            if(strcmp(s[j], q) == 0) {
                numq = j;
                break;
            }
        }
        find(nump, numq);
    }
    return 0;
}

