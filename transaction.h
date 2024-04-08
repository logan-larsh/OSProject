#ifndef TRANSACTION_H
#define TRANSACTION_H

typedef struct Transaction
{
    int user;
    char account_number[20];
    char transaction_type[20];
    int amount;
    char recipient_account[20];
} Transaction;

#endif