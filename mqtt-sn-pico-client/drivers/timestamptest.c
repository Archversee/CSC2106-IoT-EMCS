#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint32_t get_current_time()
{
    time_t now;
    time(&now);
    printf("timet format: %d\n", (int) now);
    // in hexadecimal
    printf("hex format: 0x%08X\n", (int) now);
    return (uint32_t) now;
}

int main()
{
    get_current_time();
    return 0;
}
