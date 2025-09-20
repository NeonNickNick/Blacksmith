#include "realrandom.h"

#include <stdio.h>
#include <windows.h>

#if defined(_WIN32) || defined(_WIN64)
#include <wincrypt.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

void init_rand() {
    unsigned int seed;


#if defined(_WIN32) || defined(_WIN64)
    HCRYPTPROV hProvider;
    if (CryptAcquireContext(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        CryptGenRandom(hProvider, sizeof(seed), (BYTE *) &seed);
        CryptReleaseContext(hProvider, 0);
    } else {

        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        seed = ft.dwHighDateTime ^ ft.dwLowDateTime;
    }

#else
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd != -1) {
        read(fd, &seed, sizeof(seed));
        close(fd);
    } else {

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        seed = ts.tv_nsec ^ (ts.tv_sec << 20);
    }
#endif

    srand(seed);
}
double myrandom() {
    return (double) rand() / (RAND_MAX + 1.0);
}