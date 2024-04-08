/*
Group E
Author Name: Joshua Hernandez
Email: joshua.r.hernandez@okstate.edu
Date: 04/07/2024
*/

#include <pthread.h>
#include <stdio.h>

#define MAX_QUEUE_SIZE 100

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    int queue[MAX_QUEUE_SIZE];
    int front, rear;
} Monitor;

void initMonitor(Monitor *monitor) {
    // Initialize mutex
    pthread_mutex_init(&monitor->mutex, NULL);
    // Initialize condition
    pthread_cond_init(&monitor->condition, NULL);
    // Initialize queue pointers
    monitor->front = monitor->rear = -1;
}

void giveKey(Monitor *monitor) {
    // Lock mutex
    pthread_mutex_lock(&monitor->mutex);
}

void takeKey(Monitor *monitor) {
    // Unlock mutex
    pthread_mutex_unlock(&monitor->mutex);

}

void enqueueMonitorQueue(Monitor *monitor, int processId) {
    if (monitor->rear == MAX_QUEUE_SIZE - 1) {
        // Display if queue is full
        printf("Queue overflow\n");
        return;
    }
    if (monitor->front == -1)
        // Set front pointer if queue is empty
        monitor->front = 0;
    // Increment rear
    monitor->rear++;
    // Add process ID to the queue
    monitor->queue[monitor->rear] = processId;
}

void displayQueueStatus(Monitor *monitor) {
    printf("Monitor queue status:\n");
    if (monitor->front == -1)
        // Condition if queue if empty
        printf("Queue is empty\n");
    else {
        printf("Queue elements:");
        for (int i = monitor->front; i <= monitor->rear; i++)
            // Display queue
            printf(" %d", monitor->queue[i]);
        printf("\n");
    }
}

int main() {
    Monitor monitor;
    initMonitor(&monitor);

    giveKey(&monitor);

    takeKey(&monitor);

    return 0;
}