/*
Author Name: Logan Larsh
Email: logan.larsh@okstate.edu
Date: 04/07/2024
Program Description: Implementation of IPC for banker program. Still needing to add balance tracking.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "transaction.h"

#define SHMSZ 1024
#define KEY 2375 // Use the same key value for both functions

// Creates shared memory and saves tasks to it. Will add balance checking and logic.
void log_transaction(Transaction *transaction)
{
    int shmid;
    key_t key = KEY;
    char *shm;

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        exit(1);
    }

    size_t current_length = strlen(shm);
    shm += current_length;

    sprintf(shm, "Transaction type: %s, Account: %s, Amount: %d, Recipient: %s, user: %d\n",
            transaction->transaction_type,
            transaction->account_number,
            transaction->amount,
            transaction->recipient_account, transaction->user);

    shmdt(shm);
}

// Prints out the balances and commands executed upon process completion.
void read_shared_memory()
{
    int shmid;
    key_t key = KEY;
    char *shm;

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (char *)-1)
    {
        perror("shmat");
        exit(1);
    }

    printf("Shared Memory Contents:\n%s\n", shm);
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
}