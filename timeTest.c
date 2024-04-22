#include <stdio.h>
#include <time.h>

int main() {
    time_t current_time;
    struct tm *local_time;

    current_time = time(NULL);
    local_time = localtime(&current_time);

    printf("Current date and time: %s", asctime(local_time));

    return 0;
}