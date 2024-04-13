
/*
Author Name: Cole Aldridge
Email: cole.aldridge@okstate.edu
Date: 04/07/2024
Program Description: Default driver.c file is utilized to read in the commands list and instiantiate the 
process manager.
*/

#include <stdio.h>
#include "taskCreate.c"

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