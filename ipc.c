/*
Group E
Author Name: Logan Larsh
Email: logan.larsh@okstate.edu
Date: 04/07/2024
Program Description: CS 4323 Group Project
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include "transaction.h"

#define SHMSZ 10240
#define KEY 9997 // Use the same key value for both functions
#define KEY_TRANSACTIONS 9998
#define KEY_EXECUTEDCOMMANDS 9999

time_t current_time;
struct tm *local_time;

//Cleans shared memory
void clear_shared_memory(key_t key) {
    int shmid;
    if ((shmid = shmget(key, 0, 0)) != -1) {
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl (IPC_RMID)");
            exit(1);
        }
    }
}

//Cleans the strings in memory
void remove_blank_lines(char *str) {
    int i, j;
    int len = strlen(str);


    if (len > 0 && str[0] == '\n') {
        for (j = 0; j < len; j++) {
            str[j] = str[j+1];
        }
        len--;
    }

    for (i = 0; i < len; i++) {

        if (str[i] == '\n') {

            if (str[i+1] == '\n') {

                for (j = i + 1; j < len; j++) {
                    str[j] = str[j+1];
                }
                len--;
                i--;
            }
        }
    }
}

//Cleans the strings in memory
void add_newline(char *str) {
    int len = strlen(str);
    char result[2 * len];
    int j = 0;

    for (int i = 0; i < len; i++) {
        result[j++] = str[i];
        if (strncmp(str + i + 1, "Account", 7) == 0) {
            if (i == 0 || str[i - 1] != '\n') {
                result[j++] = '\n';
            }
        }
    }
    result[j] = '\0';
    strcpy(str, result);
}

//The following functions are the method calls for the types of transactions.  
//If you scroll down to the log_transaction it's the where the actuall calling occurs. (Good place for waits to test deadlock)
//------------------------------------------------------------------------------------------------------------------

//Handles the logic of depositing funds to the user balance.
void deposit(Transaction *transaction, char *shm_accounts, char *shm_executions, int current_length_executions){
    current_time = time(NULL);
    local_time = localtime(&current_time);
    int current_balance = 0;

    //Assign account info to a temporary string.
    char *account_start = strstr(shm_accounts, transaction->account_number);



    if (account_start != NULL) {

        //Manipulate strings in memory to isolate desired balance and remove current balance from shared memory to be added back in after modifications.
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

        sscanf(account_start, "%*[^:]: %d", &current_balance);

        memmove(start, end, remaining_length + 1);

        

        remove_blank_lines(shm_accounts);

        size_t current_length_accounts = strlen(shm_accounts);

        shm_executions += current_length_executions;



        
        //Apply modificatinos to shared memory
        sprintf(shm_executions, "Deposit initiated in amount of %d to account %s with balance of %d | %s", 
            transaction->amount, transaction->account_number, current_balance, asctime(local_time));
        current_balance += transaction->amount;
        sprintf(shm_accounts + current_length_accounts, "Account: %s, Balance: %d\n",
                transaction->account_number, current_balance);
        

    } else {
        shm_executions += current_length_executions;

        sprintf(shm_executions, "Can't Deposit to Account: Account number does not exist: %s | %s", transaction->account_number, asctime(local_time));

    }
}

//Deposit overloader to point to a recipient. Is used by the transfer logic.
void tranDeposit(Transaction *transaction, char *shm_accounts, char *shm_executions, int current_length_executions){
    current_time = time(NULL);
    local_time = localtime(&current_time);
    int current_balance = 0;

    //Assign account info to a temporary string.
    char *account_start = strstr(shm_accounts, transaction->recipient_account);


    if (account_start != NULL) {
        //Manipulate strings in memory to isolate desired balance and remove current balance from shared memory to be added back in after modifications.

        char *start = account_start;
        while (start > shm_accounts && *start != '\n') {
            start--;
        }
        if (start > shm_accounts) {
            start++;
        }

        char *end = account_start + strlen(transaction->recipient_account);
        while (*end != '\n' && *end != '\0') {
            end++;
        }

        size_t section_length = end - start;
        size_t remaining_length = strlen(shm_accounts) - section_length;

        sscanf(account_start, "%*[^:]: %d", &current_balance);

        memmove(start, end, remaining_length + 1);

        remove_blank_lines(shm_accounts);

        size_t current_length_accounts = strlen(shm_accounts);

        shm_executions += current_length_executions;

        
        
        sprintf(shm_executions, "Deposit initiated in amount of %d to account %s with balance of %d | %s", 
            transaction->amount, transaction->recipient_account, current_balance, asctime(local_time));
        current_balance += transaction->amount;
        sprintf(shm_accounts + current_length_accounts, "Account: %s, Balance: %d\n",
                transaction->recipient_account, current_balance);
        

    }
}

//Perform logic of deducing funds from an account in shared memory
void withdraw(Transaction *transaction, char *shm_accounts, char *shm_executions, int current_length_executions){
    current_time = time(NULL);
    local_time = localtime(&current_time);
    int current_balance = 0;

    //Assign account info to a temporary string.
    char *account_start = strstr(shm_accounts, transaction->account_number);


    if (account_start != NULL) {
        //Manipulate strings in memory to isolate desired balance and remove current balance from shared memory to be added back in after modifications.

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

        sscanf(account_start, "%*[^:]: %d", &current_balance);

        memmove(start, end, remaining_length + 1);

        remove_blank_lines(shm_accounts);

        size_t current_length_accounts = strlen(shm_accounts);

        shm_executions += current_length_executions;

        
        
        sprintf(shm_executions, "Withdraw initiated in amount of %d from account %s with balance of %d | %s", 
            transaction->amount, transaction->account_number, current_balance, asctime(local_time));
        current_balance -= transaction->amount;
        sprintf(shm_accounts + current_length_accounts, "Account: %s, Balance: %d\n",
                transaction->account_number, current_balance);
        

    } else {
        shm_executions += current_length_executions;

        sprintf(shm_executions, "Can't Withdraw From Account: Account number does not exist: %s | %s", transaction->account_number, asctime(local_time));

    }
}

//Deletes an account from shared memory and returns the existing account balance for that account.
void close_account(Transaction *transaction, char *shm_accounts, char *shm_executions, int current_length_executions){
    current_time = time(NULL);
    local_time = localtime(&current_time);
    int current_balance = 0;

    //Assign account info to a temporary string.
    char *account_start = strstr(shm_accounts, transaction->account_number);


    if (account_start != NULL) {
        //Manipulate strings in memory to isolate desired balance and remove current balance from shared memory to be added back in after modifications.

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

        sscanf(account_start, "%*[^:]: %d", &current_balance);

        memmove(start, end, remaining_length + 1);

        remove_blank_lines(shm_accounts);

        size_t current_length_accounts = strlen(shm_accounts);

        shm_executions += current_length_executions;
        sprintf(shm_executions, "Succesfully Closed account %s with Balance = %d | %s", transaction->account_number, current_balance, asctime(local_time));
        

    } else {
        shm_executions += current_length_executions;

        sprintf(shm_executions, "Can't Close Account: Account number does not exist: %s | %s", transaction->account_number, asctime(local_time));

    }
}

//------------------------------------------------------------------
//Core logic that the processes call on.
void log_transaction(Transaction *transaction)
{
    current_time = time(NULL);
    local_time = localtime(&current_time);
    int shmid, shmid_accounts, shmid_executions;
    key_t key = KEY;
    key_t key_accounts = KEY_TRANSACTIONS;
    key_t key_executions = KEY_EXECUTEDCOMMANDS;
    char *shm, *shm_accounts, *shm_executions;

    //Attach to shared memory.
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
    
    //Log raw data in shared memory.
    sprintf(shm, "Transaction type: %s, Account: %s, Amount: %d, Recipient: %s, user: %d\n",
            transaction->transaction_type,
            transaction->account_number,
            transaction->amount,
            transaction->recipient_account, transaction->user);

    //The following if else block checks for the transaction type and performs appropriate logic and method calls.
    if (strcmp(transaction->transaction_type, "Create") == 0) {
        //while(1);
        char *account_start = strstr(shm_accounts, transaction->account_number);

        //Create account with starting balance if it does not already exist.
        if (account_start == NULL) {
            size_t current_length_accounts = strlen(shm_accounts);
            shm_executions += current_length_executions;
            shm_accounts += current_length_accounts;

            sprintf(shm_executions, "Account %s created with initial balance of %d | %s",
                transaction->account_number, transaction->amount, asctime(local_time));
            sprintf(shm_accounts, "Account: %s, Balance: %d\n",
                    transaction->account_number, transaction->amount);

        } else {

            sprintf(shm_executions, "Can't Create Account: Account number already exists: %s | %s", transaction->account_number, asctime(local_time));

        }
    }
    else if (strcmp(transaction->transaction_type, "Deposit") == 0) {
        //while(1);
        deposit(transaction, shm_accounts, shm_executions, current_length_executions);
        
    }
    else if (strcmp(transaction->transaction_type, "Withdraw") == 0) {
        //while(1);
        withdraw(transaction, shm_accounts, shm_executions, current_length_executions);
    }
    else if (strcmp(transaction->transaction_type, "Transfer") == 0) {
        //while(1);
        int current_balance = 0;
        char *account_start = strstr(shm_accounts, transaction->account_number);
        char *recipient_start = strstr(shm_accounts, transaction->recipient_account);


        if (account_start != NULL && recipient_start != NULL){
            //Prints information about transfer to shared memory and performs a parallel deposit/withdraw if the accounts exist.

            shm_executions += current_length_executions;

            sprintf(shm_executions, "** Transfer initiated in amount of %d from account %s to account %s\nBegin account actions now ->", 
                transaction->amount, transaction->account_number, transaction->recipient_account);

            withdraw(transaction, shm_accounts, shm_executions, current_length_executions);
            tranDeposit(transaction, shm_accounts, shm_executions, current_length_executions);

            current_length_executions = strlen(shm_executions);
            shm_executions += current_length_executions;
            sprintf(shm_executions, "<- Transfer Complete | %s", asctime(local_time));

        } else {
            shm_executions += current_length_executions;

            sprintf(shm_executions, "Can't Transfer from Account: Account number/s do not exist | %s", asctime(local_time));

        }
    }
    else if (strcmp(transaction->transaction_type, "Inquiry") == 0) {
        //while(1);

        //Used to print out the current contents of a balance to shared memory.
        int current_balance = 0;
        char *account_start = strstr(shm_accounts, transaction->account_number);


        if (account_start != NULL){

            shm_executions += current_length_executions;
            sscanf(account_start, "%*[^:]: %d", &current_balance);
            sprintf(shm_executions, "Inquiry Results for acount %s: %d | %s", transaction->account_number, current_balance, asctime(local_time));
            

        } else {
            shm_executions += current_length_executions;

            sprintf(shm_executions, "Inquiry Failed, Account Number Not Found: %s | %s", transaction->account_number, asctime(local_time));

        }
    }
    else if (strcmp(transaction->transaction_type, "Close") == 0) {
        //while(1);

        close_account(transaction, shm_accounts, shm_executions, current_length_executions);
    }

    shmdt(shm);
    shmdt(shm_accounts);
    shmdt(shm_executions);
}


//The following logic block prints out all of the shared memories at program conclusion.
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

    printf("\nRaw Data Contents:\n%s\n", shm);
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

    printf("Transactions Executed:\n%s\n", shm_executions);
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

    remove_blank_lines(shm_accounts);
    add_newline(shm_accounts);
    remove_blank_lines(shm_accounts);

    printf("Account Results:\n%s\n", shm_accounts);
    shmdt(shm_accounts);
    shmctl(shmid_accounts, IPC_RMID, NULL);
}

