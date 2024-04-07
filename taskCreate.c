#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_TRANSACTIONS 1000
#define MAX_LINE_LENGTH 100

typedef struct Transaction {
    char account_number[20]; 
    char transaction_type[20];
    int amount;
    char recipient_account[20]; 
} Transaction;


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
    // while (fgets(line, MAX_LINE_LENGTH, fp))
    // {
    //     int account_number;
    //     char transaction_type[20];
    //     int amount;
    //     int recipient_account;

    //     if (sscanf(line, "%d %s %d %d", &account_number, transaction_type, &amount, &recipient_account) == 4)
    //     {
    //         transactions[*num_transactions].account_number = account_number;
    //         strcpy(transactions[*num_transactions].transaction_type, transaction_type);
    //         transactions[*num_transactions].amount = amount;
    //         transactions[*num_transactions].recipient_account = recipient_account;
    //         (*num_transactions)++;
    //     }
    //     else if (sscanf(line, "%d %s %d", &account_number, transaction_type, &amount) == 3)
    //     {
    //         transactions[*num_transactions].account_number = account_number;
    //         strcpy(transactions[*num_transactions].transaction_type, transaction_type);
    //         transactions[*num_transactions].amount = amount;
    //         transactions[*num_transactions].recipient_account = -1;
    //         (*num_transactions)++;
    //     }
    //     else if (sscanf(line, "%d %s", &account_number, transaction_type) == 2)
    //     {
    //         transactions[*num_transactions].account_number = account_number;
    //         strcpy(transactions[*num_transactions].transaction_type, transaction_type);
    //         transactions[*num_transactions].amount = 0;
    //         transactions[*num_transactions].recipient_account = -1;
    //         (*num_transactions)++;
    //     }
    //     else
    //     {
    //         printf("Failed to parse line: %s\n", line);
    //         continue; // Skip this line if it doesn't match any format.
    //     }
    // }
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

        (*num_transactions)++;
        printf("Processed transaction for account number: %s, Type: %s\n", account_number, transaction_type);
    }
    fclose(fp);
    return 1;
}

void create_child_processes(int num_users, Transaction transactions[], int num_transactions)
{
    for (int i = 0; i < num_users; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process
            printf("User %d started.\n", i + 1);
            // Perform transactions for this user
            printf("User %d amount.\n", transactions[0].amount);
            for (int j = 0; j < num_transactions; j++)
            {
                // if (transactions[j].account_number == i + 1)
                // {
                    printf("Transaction type: %s, Account: %s, Amount: %d, Recipient: %s\n",
                           transactions[j].transaction_type,
                           transactions[j].account_number,
                           transactions[j].amount,
                           transactions[j].recipient_account);
                    // Perform the transaction
                //}
            }
            printf("User %d finished.\n", i + 1);
            exit(0);
        }
        else if (pid > 0)
        {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
        else
        {
            printf("Fork failed.\n");
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    int num_users;
    Transaction transactions[MAX_TRANSACTIONS];
    int num_transactions;

    if (!read_input_file(argv[1], &num_users, transactions, &num_transactions))
    {
        return 1;
    }

    create_child_processes(num_users, transactions, num_transactions);

    return 0;
}