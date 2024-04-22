
/*
Author Name: Group E
Date: 04/07/2024
Program Description: Default driver.c file is utilized to read in the commands list and instiantiate the 
process manager.
*/

#include <stdio.h>
#include "taskCreate.c"

int main(int argc, char *argv[])
{
    // Check if the number of command-line arguments is exactly two (program name and one additional argument).
    if (argc != 2)
    {
        // If not, print the correct usage of the program to inform the user how to run the program correctly.
        printf("Usage: %s <input_file>\n", argv[0]);
        // Return 1 to indicate an error has occurred, as the program needs exactly one argument besides the program name.
        return 1;
    }

    // Declare variables to hold the number of users and an array to store transaction details.
    int num_users;
    Transaction transactions[MAX_TRANSACTIONS];  // Array to hold transactions with a size limit as defined by MAX_TRANSACTIONS.
    int num_transactions;  // Variable to keep track of the actual number of transactions read.

    // Call the function to read input from a file specified by the user. The filename is passed as argv[1].
    if (!read_input_file(argv[1], &num_users, transactions, &num_transactions))
    {
        // If the function returns false (0), indicating failure in reading the file, return 1 to indicate an error.
        return 1;
    }

    // Call the function to create child processes for each user and process the transactions as read from the file.
    create_child_processes(num_users, transactions, num_transactions);

    // Return 0 to indicate successful execution of the program.
    return 0;
}