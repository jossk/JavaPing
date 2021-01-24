#include <stdio.h>

#include "ping.h"

int main() {
    printf("Hello, World!\n");

    ping4("ukr.net", 3);
    ping4("8.8.8.8", 3);
    ping4("ua-football1.com", 3);
    ping4("workupload.com", 1);

    return 0;
}
