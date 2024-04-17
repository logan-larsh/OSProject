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

// Function to initialize monitor
void initMonitor(Monitor *monitor) {
    // Initialize mutex
    pthread_mutex_init(&monitor->mutex, NULL);
    // Initialize condition
    pthread_cond_init(&monitor->condition, NULL);
    // Initialize queue pointers
    monitor->front = monitor->rear = -1;
}

// Function to give the key
void giveKey(Monitor *monitor) {
    // Lock mutex
    pthread_mutex_lock(&monitor->mutex);
}

// Function to take the key
void takeKey(Monitor *monitor) {
    // Unlock mutex
    pthread_mutex_unlock(&monitor->mutex);
}

// Function to enqueue an element
void enqueueMonitorQueue(Monitor *monitor, int processId) {
    if (monitor->rear == MAX_QUEUE_SIZE - 1) {
        // Display if queue is full
        printf("Queue is full.\n");
        return;
    }

    // Increment rear
    monitor->rear++;
    
    // Add process ID to the queue
    monitor->queue[monitor->rear] = processId;

    if (monitor->front == -1) {
        // Set front pointer if queue is empty
        monitor->front = 0;
    }
}

// Function to dequeue an element
void dequeueMonitorQueue(Monitor *monitor) {
    if (monitor->front == -1) {
        printf("Queue is full.\n");
        return;
    }
    if (monitor->front == monitor->rear) {
        // Reset queue pointers if only one element in queue
        monitor->front = monitor->rear = -1;
    } else {
        // Shift elements to the left to remove the front element
        for (int i = monitor->front; i < monitor->rear; i++) {
            monitor->queue[i] = monitor->queue[i + 1];
        }
        monitor->rear--;
    }
}

// Function to display the queue status
void displayQueueStatus(Monitor *monitor) {
    printf("Monitor queue status:\n");
    if (monitor->front == -1) {
        // Condition if queue if empty
        printf("Queue is empty\n");
    } else {
        printf("Queue elements:");
        for (int i = monitor->front; i <= monitor->rear; i++)
            // Display queue
            printf(" %d", monitor->queue[i]);
        printf("\n");
    }
}