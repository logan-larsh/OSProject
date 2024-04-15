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

#define SHMSZ 10240
#define KEY 9997 // Use the same key value for both functions
#define KEY_TRANSACTIONS 9998
#define KEY_EXECUTEDCOMMANDS 9999

// Creates shared memory and saves tasks to it. Will add balance checking and logic.
void clear_shared_memory(key_t key) {
    int shmid;
    if ((shmid = shmget(key, 0, 0)) != -1) {
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl (IPC_RMID)");
            exit(1);
        }
    }
}

void remove_blank_lines(char *str) {
    int i, j;
    int len = strlen(str);

    // Remove \n at the beginning of the string if present
    if (len > 0 && str[0] == '\n') {
        for (j = 0; j < len; j++) {
            str[j] = str[j+1];
        }
        len--;
    }

    // Iterate through the string
    for (i = 0; i < len; i++) {
        // If current character is a newline
        if (str[i] == '\n') {
            // Check if the next character is also a newline
            if (str[i+1] == '\n') {
                // Remove the extra newline character
                for (j = i + 1; j < len; j++) {
                    str[j] = str[j+1];
                }
                len--; // Decrement length of string
                i--;   // Move back one step to recheck the same position
            }
        }
    }
}

void log_transaction(Transaction *transaction)
{
    int shmid, shmid_accounts, shmid_executions;
    key_t key = KEY;
    key_t key_accounts = KEY_TRANSACTIONS;
    key_t key_executions = KEY_EXECUTEDCOMMANDS;
    char *shm, *shm_accounts, *shm_executions;

    

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

    if ((shmid_accounts = shmget(key_accounts, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget (accounts)");
        exit(1);
    }

    if ((shm_accounts = shmat(shmid_accounts, NULL, 0)) == (char *)-1) {
        perror("shmat (accounts)");
        exit(1);
    }

    if ((shmid_executions = shmget(key_executions, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget (executions)");
        exit(1);
    }

    if ((shm_executions = shmat(shmid_executions, NULL, 0)) == (char *)-1) {
        perror("shmat (executions)");
        exit(1);
    }

    size_t current_length = strlen(shm);
    
    size_t current_length_executions = strlen(shm_executions);
    shm += current_length;
    

    sprintf(shm, "Transaction type: %s, Account: %s, Amount: %d, Recipient: %s, user: %d\n",
            transaction->transaction_type,
            transaction->account_number,
            transaction->amount,
            transaction->recipient_account, transaction->user);

    if (strcmp(transaction->transaction_type, "Create") == 0) {

        char *account_start = strstr(shm_accounts, transaction->account_number);

        if (account_start == NULL) {
            size_t current_length_accounts = strlen(shm_accounts);
            shm_executions += current_length_executions;
            shm_accounts += current_length_accounts;

            sprintf(shm_executions, "Account %s created with initial balance of %d\n",
                transaction->account_number, transaction->amount);
            sprintf(shm_accounts, "Account: %s, Balance: %d\n",
                    transaction->account_number, transaction->amount);

        } else {

            sprintf(shm_executions, "Can't Create Account: Account number already exists: %s\n", transaction->account_number);

        }
    }
    else if (strcmp(transaction->transaction_type, "Deposit") == 0) {

        int current_balance = 0;
        char *account_start = strstr(shm_accounts, transaction->account_number);
        printf("Deposit command ref: %s\n", account_start);
        sscanf(account_start, "%*[^:]: %d", &current_balance);


        if (account_start != NULL) {

            char *start = account_start;
            while (start > shm_accounts && *start != '\n') {
                start--;
            }
            if (start > shm_accounts) {
                start++;
            }

            char *end = account_start + strlen(transaction->account_number);
            while (*end != '\n' && *end != '\0') {
                end++;
            }

            size_t section_length = end - start;
            size_t remaining_length = strlen(shm_accounts) - section_length;
            memmove(start, end, remaining_length + 1);

            remove_blank_lines(shm_accounts);

            size_t current_length_accounts = strlen(shm_accounts);

            printf("%s", shm_accounts);

            shm_executions += current_length_executions;
            
            sprintf(shm_executions, "Deposit initiated in amount of %d to account %s with balance of %d\n", 
                transaction->amount, transaction->account_number, current_balance);
            current_balance += transaction->amount;
            sprintf(shm_accounts + current_length_accounts, "Account: %s, Balance: %d",
                    transaction->account_number, current_balance);
            

        } else {
            shm_executions += current_length_executions;

            sprintf(shm_executions, "Can't Deposit to Account: Account number does not exist: %s\n", transaction->account_number);

        }
    }
    else if (strcmp(transaction->transaction_type, "Withdraw") == 0) {

        int current_balance = 0;
        char *account_start = strstr(shm_accounts, transaction->account_number);
        printf("Withdraw command ref: %s\n", account_start);
        sscanf(account_start, "%*[^:]: %d", &current_balance);


        if (account_start != NULL) {

            char *start = account_start;
            while (start > shm_accounts && *start != '\n') {
                start--;
            }
            if (start > shm_accounts) {
                start++;
            }

            char *end = account_start + strlen(transaction->account_number);
            while (*end != '\n' && *end != '\0') {
                end++;
            }

            size_t section_length = end - start;
            size_t remaining_length = strlen(shm_accounts) - section_length;
            memmove(start, end, remaining_length + 1);

            remove_blank_lines(shm_accounts);

            size_t current_length_accounts = strlen(shm_accounts);

            printf("%s", shm_accounts);

            shm_executions += current_length_executions;
            
            sprintf(shm_executions, "Withdraw initiated in amount of %d from account %s with balance of %d\n", 
                transaction->amount, transaction->account_number, current_balance);
            current_balance -= transaction->amount;
            sprintf(shm_accounts + current_length_accounts, "Account: %s, Balance: %d",
                    transaction->account_number, current_balance);

        } else {
            shm_executions += current_length_executions;

            sprintf(shm_executions, "Can't Withdraw from Account: Account number does not exist: %s\n", transaction->account_number);

        }
    }


    shmdt(shm);
    shmdt(shm_accounts);
    shmdt(shm_executions);

    usleep(1000);
}

// Prints out the balances and commands executed upon process completion.
void read_shared_memory()
{
    int shmid, shmid_accounts, shmid_executions;
    key_t key = KEY;
    key_t key_accounts = KEY_TRANSACTIONS;
    key_t key_executions = KEY_EXECUTEDCOMMANDS;
    char *shm, *shm_accounts, *shm_executions;

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

    printf("\nShared Memory Contents:\n%s\n", shm);
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    if ((shmid_executions = shmget(key_executions, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget (executions)");
        exit(1);
    }

    if ((shm_executions = shmat(shmid_executions, NULL, 0)) == (char *)-1) {
        perror("shmat (executions)");
        exit(1);
    }

    printf("Transactions Executed:\n%s\n", shm);
    shmdt(shm_executions);
    shmctl(shmid_executions, IPC_RMID, NULL);

    if ((shmid_accounts = shmget(key_accounts, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget (accounts)");
        exit(1);
    }

    if ((shm_accounts = shmat(shmid_accounts, NULL, 0)) == (char *)-1) {
        perror("shmat (accounts)");
        exit(1);
    }

    printf("Account Results:\n%s\n", shm);
    shmdt(shm_accounts);
    shmctl(shmid_accounts, IPC_RMID, NULL);
}