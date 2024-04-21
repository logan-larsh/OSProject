/*
Group: E
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
#define SEM_NAME "/sem_synchronization"
// Define timeout duration for 3 seconds
#define TIMEOUT_DURATION 3

// Global variable to store the PID of the current process
pid_t currentProcess = -1;

// Define monitor structure
typedef struct {
    sem_t *mutex;
    int queue[MAX_QUEUE_SIZE];
    int front, rear;
} Monitor;

// Function to clean up monitor
void cleanupMonitor(Monitor *monitor) {
    sem_close(monitor->mutex);
    sem_unlink(SEM_NAME);
}

// Function to initialize monitor
void initMonitor(Monitor *monitor) {
    // Clean up previous semaphore
    cleanupMonitor(monitor);
    // Create semaphore
    monitor->mutex = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (monitor->mutex == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    monitor->front = monitor->rear = -1;
}

// Signal handler for SIGALRM
void handleTimeout(pid_t pid, Monitor *monitor) {
    // Check if process is holding key
    if (pid != -1) {
        printf("Process %d exceeded timeout. Terminating...\n", pid);
        // Terminate process
        kill(pid, SIGKILL); 
    }
    if (sem_post(monitor->mutex) == -1) {
        perror("sem_post");
        exit(1);
    }
    exit(0);
}

clock_t start, end;
double cpu_time_used;

// Function to start the timer
void clockStart(){
    start = clock();
}

// Function to check if the timeout has been exceeded
void timeoutCheck(pid_t pid, Monitor *monitor){
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    if(cpu_time_used > TIMEOUT_DURATION){
        handleTimeout(pid, monitor);
    }
}

// Function to give key
void giveKey(Monitor *monitor) {
    if (sem_wait(monitor->mutex) == -1) {
        perror("sem_wait");
        exit(1);
    }
}


// Function to take key
void takeKey(Monitor *monitor) {
    if (sem_post(monitor->mutex) == -1) {
        perror("sem_post");
        exit(1);
    }
}

// Function to enqueue an element
void enqueueMonitorQueue(Monitor *monitor, int processId) {
    // Give the key
    giveKey(monitor);

    // Check for queueu overflow
    if (monitor->rear == MAX_QUEUE_SIZE - 1) {
        printf("Queue overflow\n");
        // Take the key
        takeKey(monitor);
        return;
    }

    if (monitor->front == -1)
        monitor->front = 0;

    monitor->rear++;
    monitor->queue[monitor->rear] = processId;

    // Take the key
    takeKey(monitor);
}

// Funtion to dequeue an element
void dequeueMonitorQueue(Monitor *monitor) {
    // Give the key
    giveKey(monitor);

    // Check for queue underflow
    if (monitor->front == -1) {
        printf("Queue underflow\n");
        // Take the key
        takeKey(monitor);
        return;
    }

    // Assign process ID of element to dequeue
    int dequeuedProcessId = monitor->queue[monitor->front];
    
    if (monitor->front == monitor->rear) {
        // Reset queue pointers if there's only one element in the queue
        monitor->front = monitor->rear = -1;
    } else {
        // Move front pointer to the next element
        monitor->front++;
    }
    
    // Take the key
    takeKey(monitor);
}

// Function to display the queue status
void displayQueueStatus(Monitor *monitor) {
    // Give the key
    giveKey(monitor);

    printf("Monitor queue status:\n");
    if (monitor->front == -1)
        printf("Queue is empty\n");
    else {
        printf("Queue elements:");
        for (int i = monitor->front; i <= monitor->rear; i++)
            printf(" %d", monitor->queue[i]);
        printf("\n");
    }

    // Take the key
    takeKey(monitor);
}

