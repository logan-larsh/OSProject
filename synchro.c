/*
Group E
Author Name: Joshua Hernandez
Email: joshua.r.hernandez@okstate.edu
Date: 04/07/2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>

#define MAX_QUEUE_SIZE 100
#define SEM_NAME "/my_sem"

typedef struct {
    sem_t *mutex;
    int queue[MAX_QUEUE_SIZE];
    int front, rear;
} Monitor;

// Function to initialize monitor
void initMonitor(Monitor *monitor) {
    monitor->mutex = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (monitor->mutex == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    monitor->front = monitor->rear = -1;
}

// Function to give the key
void giveKey(Monitor *monitor) {

    if (sem_wait(monitor->mutex) == -1) {
        perror("sem_wait");
        exit(1);
    }
}

void takeKey(Monitor *monitor) {

    if (sem_post(monitor->mutex) == -1) {
        perror("sem_post");
        exit(1);
    }
}

void enqueueMonitorQueue(Monitor *monitor, int processId) {
    giveKey(monitor);

    if (monitor->rear == MAX_QUEUE_SIZE - 1) {

        printf("Queue overflow\n");
        takeKey(monitor);
        return;
    }

    if (monitor->front == -1)
        monitor->front = 0;

    monitor->rear++;
    monitor->queue[monitor->rear] = processId;

    takeKey(monitor);
}

void displayQueueStatus(Monitor *monitor) {
    giveKey(monitor);

    printf("Monitor queue status:\n");
    if (monitor->front == -1)
        printf("Queue is empty\n");
    } else {
        printf("Queue elements:");
        for (int i = monitor->front; i <= monitor->rear; i++)
            printf(" %d", monitor->queue[i]);
        printf("\n");
    }

    takeKey(monitor);
}