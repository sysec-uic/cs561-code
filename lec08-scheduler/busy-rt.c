#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    struct sched_param param;
    int ret;

    // Set high priority for real-time scheduling.
    // SCHED_FIFO and SCHED_RR require root privileges.
    param.sched_priority = 99;  // Highest possible priority for SCHED_FIFO
    ret = sched_setscheduler(0, SCHED_FIFO, &param);
    if (ret != 0) {
        perror("sched_setscheduler failed");
        exit(EXIT_FAILURE);
    }

    printf("Running with SCHED_FIFO, priority %d\n", param.sched_priority);
    printf("WARNING: This program will consume all CPU cycles in an infinite loop.\n");

    // Infinite busy loop to consume CPU cycles.
    while (1) {
        // A busy loop to prevent optimization and fully utilize the CPU.
        volatile unsigned long long i;
        for (i = 0; i < 1000000ULL; i++) {
            // This loop is intentionally empty.
        }
    }

    return 0;
}
