/*
Group E
Author Name: Joshua Hernandez
Email: joshua.r.hernandez@okstate.edu
Date: 04/21/2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define MAX_QUEUE_SIZE 100
#define SEM_NAME "/sem"

typedef struct {
    sem_t *semaphore;
    int queue[MAX_QUEUE_SIZE];
    int front, rear;
    time_t keyAcuiredTime;
} Monitor;

Monitor monitor;
volatile sig_atomic_t alarm_triggered = 0; // Flag to track if alarm handler has been triggered

// Function to set alarm handler
void alarmHandler(int sig) {
    // Check if the monitor's queue is empty
    if (monitor.front == -1) {
        // If the queue is empty, it's a timeout situation
        printf("Process timeout. No processes in the queue. Terminating.\n");
        exit(0);
    } else {
        // If the queue is not empty, give the key to the next process
        printf("Process timeout. Moving to the next process.\n");
        dequeueMonitorQueue(&monitor);
        // Check if there are any remaining processes in the queue
        if (monitor.front != -1) {
            // If there are, give the key to the next process
            giveKey(&monitor);
        }
        alarm(60); // Reset alarm for next minute
    }
}

// Function to initialize monitor
void initMonitor(Monitor *monitor) {
    // Initialize semaphore with value of 1
    monitor->semaphore = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (monitor->semaphore == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // Initialize queue pointers
    monitor->front = monitor->rear = -1;
}

// Function to give the key
void giveKey(Monitor *monitor) {
    // Give the semaphore 
    if (sem_wait(monitor->semaphore) == -1) {
        perror("sem_wait");
        exit(1);
    }
    monitor->keyAcuiredTime = time(NULL);
}

void takeKey(Monitor *monitor) {
    // Take the semaphore
    if (sem_post(monitor->semaphore) == -1) {
        perror("sem_post");
        exit(1);
    }
}

void enqueueMonitorQueue(Monitor *monitor, int processId) {
    // Give semaphore
    giveKey(monitor);

    // Check for queue overflow
    if (monitor->rear == MAX_QUEUE_SIZE - 1) {
        printf("Queue overflow\n");
        // Take semaphore
        takeKey(monitor);
        return;
    }

    if (monitor->front == -1)
        monitor->front = 0;

    monitor->rear++;
    // Enqueue the process ID
    monitor->queue[monitor->rear] = processId;

    // Take semaphore
    takeKey(monitor);
}

// Function to dequeue an element
void dequeueMonitorQueue(Monitor *monitor) {
    // Give semaphore
    giveKey(monitor);

    // Check if queue is empty
    if (monitor->front == -1) {
        printf("Queue underflow\n");
        takeKey(monitor);
        return;
    }

    
    if (monitor->front == monitor->rear) {
        // Reset queue pointers if there is one element
        monitor->front = monitor->rear = -1;
    } else {
        // Shfit elements to the left
        for (int i = monitor->front; i < monitor->rear; i++) {
            monitor->queue[i] = monitor->queue[i + 1];
        }
        monitor->rear--;
    }

    //monitor->keyAcuiredTime = time(NULL);
    takeKey(monitor);
}

// Function to display queue status
void displayQueueStatus(Monitor *monitor) {
    // Give semaphore
    giveKey(monitor);

    printf("Monitor queue status:\n");
    if (monitor->front == -1) {
        printf("Queue is empty\n");
    } else {
        printf("Queue elements:");
        for (int i = monitor->front; i <= monitor->rear; i++)
            printf(" %d", monitor->queue[i]);
        printf("\n");
    }

    // Take semaphore
    takeKey(monitor);
}