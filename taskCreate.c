/*
Group E
Author Name: Cole Aldridge
Email: cole.aldridge@okstate.edu
Date: 04/07/2024
Program Description: CS 4323 Group Project
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include "synchro.c"
#include "ipc.c"
#include "transaction.h"
#include <sys/mman.h>

#define MAX_USERS 100
#define MAX_TRANSACTIONS 1000
#define MAX_LINE_LENGTH 100

Monitor *monitor;

bool value_exists(char arr[][20], int size, char *value)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(arr[i], value) == 0)
        {
            return true; // Value found
        }
    }
    return false; // Value not found
}

int find_index(char userArray[][20], int num_users, char* value) {
    for (int i = 0; i < num_users; i++) {
        if (strcmp(userArray[i], value) == 0) {
            return i; // Return the index where the value is found
        }
    }
    return -1; // Return -1 if the value is not found
}

void printUserArray(char userArray[][20], int numUniqueUsers)
{
    printf("Unique User Account Numbers:\n");
    for (int i = 0; i < numUniqueUsers; i++)
    {
        printf("%d: %s\n", i + 1, userArray[i]);
    }
}

int read_input_file(char *input_file, int *num_users, Transaction transactions[], int *num_transactions)
{
    FILE *fp = fopen(input_file, "r");
    if (fp == NULL)
    {
        printf("Error opening file: %s\n", input_file);
        return 0;
    }

    fscanf(fp, "%d", num_users);

    *num_transactions = 0;
    char line[MAX_LINE_LENGTH];
    char userArray[*num_users][20]; // Array of strings
    int numUniqueUsers = 0;         // Track the number of unique users

    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        char account_number[20];
        char transaction_type[20];
        int amount = 0;
        char recipient_account[20] = "";

        // First, try to parse lines with all four elements: account, transaction, amount, recipient
        if (sscanf(line, "%s %s %d %s", account_number, transaction_type, &amount, recipient_account) == 4)
        {
            // This branch is for transfers with a recipient account.
        }
        else if (sscanf(line, "%s %s %d", account_number, transaction_type, &amount) == 3)
        {
            // For create, deposit, withdraw where there's an amount but no recipient account.
            recipient_account[0] = '\0'; // Make sure recipient_account is clearly empty
        }
        else if (sscanf(line, "%s %s", account_number, transaction_type) == 2)
        {
            // For inquiries and close, which don't have an amount or recipient.
            amount = 0;                  // Ensuring amount is zero
            recipient_account[0] = '\0'; // Ensuring recipient account is empty
        }
        else
        {
            printf("Failed to parse line: %s", line);
            continue; // Skip this line if it doesn't match any format.
        }

        // Assign parsed values to the current transaction object
        strcpy(transactions[*num_transactions].account_number, account_number);
        strcpy(transactions[*num_transactions].transaction_type, transaction_type);
        transactions[*num_transactions].amount = amount;
        strcpy(transactions[*num_transactions].recipient_account, recipient_account);

        if (!value_exists(userArray, numUniqueUsers, account_number))
        {
            if (numUniqueUsers < *num_users)
            {                                                        // Ensure we don't exceed allocated space
                strcpy(userArray[numUniqueUsers++], account_number); // Add new account number
                transactions[*num_transactions].user = find_index(userArray, numUniqueUsers, account_number);;

            }
        } else {
            transactions[*num_transactions].user = find_index(userArray, numUniqueUsers, account_number);
        }
        (*num_transactions)++;
    }

    fclose(fp);
    return 1;
}


void perform_transaction(Transaction *transaction) {
    printf("Transaction type: %s, Account: %s, Amount: %d, Recipient: %s, user: %d\n",
           transaction->transaction_type,
           transaction->account_number,
           transaction->amount,
           transaction->recipient_account, transaction->user);

           log_transaction(transaction);
    // Perform the transaction
}

void *child_process(void *arg) {
    // Convert the void pointer back to a Transaction pointer.
    Transaction *transaction = (Transaction *)arg;
    // Extract the user id from the transaction.
    int user_id = transaction->user;

    // Notify that this user's process has started.
    printf("User %d started.\n", user_id + 1);      

    // Add the parent process ID to the monitor's queue.
    enqueueMonitorQueue(monitor, getppid());
    // Display the current status of the queue.
    displayQueueStatus(monitor);                    
     // Grant access (or the 'key') to the process to perform operations.
    giveKey(monitor);                              

     // Fork a new process.
    pid_t pid = fork();                            
    pid_t childPid;

    pid_t waited_pid;
    int status;

     // Child process after fork
    if(pid == 0){ 
         // Get parent PID for use in child operations.                                 
        childPid = getppid();        
        // Execute the transaction logic.              
        perform_transaction(transaction);           
    }

    // Parent process after fork
    if(pid > 0){               
         // Start a timer                     
        clockStart();                              
        while(1){
            // Non-blocking wait for child process to change state.
            waited_pid = waitpid(pid, &status, WNOHANG); 

            if (waited_pid == -1) {
                 // Error handling for waitpid failure.
                perror("waitpid");                 
                exit(1);
            } else if(waited_pid == 0){
                 // Check for timeouts and handle accordingly.
                timeoutCheck(childPid, monitor);   
            }
            else{
                 // Exit if child process has finished.
                exit(0);                           
            }
        }
    }

    // Retrieve the 'key' from the monitor post-transaction.
    takeKey(monitor);                             
    // Remove the entry from the monitor's queue.
    dequeueMonitorQueue(monitor);                   
    // Notify that this user's process has finished.
    printf("User %d finished.\n", user_id + 1);     
    // Return NULL as this is a void function.
    return NULL;                                    
}

void create_child_processes(int num_users, Transaction transactions[], int num_transactions)
{
    // Initialize the monitor structure.
    initMonitor(&monitor);                          
    // Clean up any residual data in the monitor.
    cleanupMonitor(monitor);                        
    // Re-initialize the monitor.
    initMonitor(&monitor);                          
    pid_t pid;

    // Clear shared memory.
    clear_shared_memory(9997);                      
    clear_shared_memory(9998);
    clear_shared_memory(9999);

    // Loop through each user.
    for (int i = 0; i < num_users; i++)             
    {
        // Fork a new process for each user.
        pid_t pid = fork();                         

        if (pid == 0)                               
        {
            // Process each transaction for the user.
            for (int j = 0; j < num_transactions; j++)  
            {
                if (transactions[j].user == i) {
                    child_process((void *)&transactions[j]);  // Execute the transaction using child_process function.
                }
            }
             // Exit child process after handling all transactions.
            exit(0);                               
        }
         // Error handling for fork failure.
        else if (pid < 0)                          
        {
            printf("Fork failed.\n");
            exit(1);
        }
    }

    int status;
    pid_t wpid;
    // Wait for all child processes to complete.
    while ((wpid = wait(&status)) > 0);             
    if(pid != 0){
        // Read shared memory after all children are done.
        read_shared_memory();                       
    }

    // Clean up the monitor structure after all processes are done.
    cleanupMonitor(monitor);                       
}