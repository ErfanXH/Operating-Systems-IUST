#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_CUSTOMERS 5	  // number of customers
#define NUM_TRANSACTION 6 // number of transactions/threads
#define INIT_STOCK 100	  // the initial value for each person stock

// type transaction. you should pass an object of this type to the function arguments while creating thread
typedef struct
{
	double amount;
	int customer_id;
} transaction_t;

double safeBox = 1000.0; // shared variable for all customers

double customersStock[NUM_CUSTOMERS]; // 1. array to store the stock of all customers

pthread_mutex_t customersMutex[NUM_CUSTOMERS]; // 2. mutexes for each customer account

pthread_mutex_t safeBoxMutex; // 2. mutex for safe box

void *withdraw(void *arg)
{
	transaction_t *transaction = (transaction_t *)arg;
	int customerID = transaction->customer_id;
	double amount = transaction->amount;

	// pthread_mutex_t customerMutex = customersMutex[customerID];
	pthread_mutex_lock(&customersMutex[customerID]);

	// can afford amount from stock
	if (customersStock[customerID] >= amount)
	{
		customersStock[customerID] -= amount;
		printf("Withdraw:<%lf> - Customer <%d> withdrew <%lf> Successfully! New balance: <%lf>\n", amount, customerID, amount, customersStock[customerID]);
	}
	// needs to take from safe box
	else
	{
		// two possible situtations:
		// 1. customer can afford the amount from stock + safe box
		// 2. customer can't afford the amount from stock + safe box (safebox will be negative)

		// customer balance negative -> check safebox
		if (customersStock[customerID] < 0)
		{
			pthread_mutex_lock(&safeBoxMutex);
			// Transaction failed
			if (safeBox < amount)
			{
				printf("Withdraw:<%lf> - Transaction for customer <%d> and <%lf> from stock failed!\n", amount, customerID, amount);
			}
			// Withdrawed from safebox
			else
			{
				customersStock[customerID] -= amount;
				safeBox -= amount;
				printf("Withdraw:<%lf> - Customer <%d> withdrew <%lf> from safe-box Successfully! New balance: <%lf> and New Safebox: <%lf>\n", amount, customerID, amount, customersStock[customerID], safeBox);
			}
			pthread_mutex_unlock(&safeBoxMutex);
		}

		// customer balance not negative
		else
		{
			pthread_mutex_lock(&safeBoxMutex);
			// Transaction failed
			if (safeBox < amount - customersStock[customerID])
			{
				printf("Withraw:<%lf> - Transaction for customer <%d> and <%lf> from stock failed!\n", amount, customerID, amount);
			}
			// Withdrawed from safebox
			else
			{
				double diff = amount - customersStock[customerID];
				safeBox -= diff;
				customersStock[customerID] = -diff;
				printf("Withdraw:<%lf> - Customer <%d> withdrew <%lf> from stock and <%lf> from safe-box Successfully! New balance: <%lf> and New Safebox: <%lf>\n", amount, customerID, amount - diff, diff, customersStock[customerID], safeBox);
			}
			pthread_mutex_unlock(&safeBoxMutex);
		}
	}
	pthread_mutex_unlock(&customersMutex[customerID]);
}

void *deposit(void *arg)
{
	transaction_t *transaction = (transaction_t *)arg;
	int customerID = transaction->customer_id;
	double amount = transaction->amount;

	// pthread_mutex_t customerMutex = customersMutex[customerID];
	pthread_mutex_lock(&customersMutex[customerID]);

	// no dept to safe box
	if (customersStock[customerID] >= 0)
	{
		customersStock[customerID] += amount;
		printf("Deposit:<%lf> - Customer <%d> deposited <%lf> Successfully! New balance: <%lf>\n", amount, customerID, amount, customersStock[customerID]);
	}
	// dept to safe box
	else
	{
		pthread_mutex_lock(&safeBoxMutex);

		// amount less than dept -> add all to safebox
		if (amount < -customersStock[customerID])
		{
			double diff = amount + customersStock[customerID];
			safeBox += -diff;
			customersStock[customerID] += amount;
			printf("Deposit:<%lf> - Customer <%d> deposited <%lf> to stock and <%lf> to safe-box Successfully! New balance: <%lf> and New Safebox: <%lf>\n", amount, customerID, amount, -diff, customersStock[customerID], safeBox);
		}
		// amount more than dept -> add dept to safebox and rest to stock
		else
		{
			double dept = customersStock[customerID];
			safeBox += -customersStock[customerID];
			customersStock[customerID] += amount;
			printf("Deposit:<%lf> - Customer <%d> deposited <%lf> to stock and <%lf> to safe-box Successfully! New balance: <%lf> and New Safebox: <%lf>\n", amount, customerID, amount, -dept, customersStock[customerID], safeBox);
		}

		pthread_mutex_unlock(&safeBoxMutex);
	}

	pthread_mutex_unlock(&customersMutex[customerID]);
}

int main()
{
	srand(time(NULL));

	// initialize the stock of all customers with INIT_STOCK
	for (int i = 0; i < NUM_CUSTOMERS; i++)
		customersStock[i] = INIT_STOCK;

	// init mutex for each account
	for (int i = 0; i < NUM_CUSTOMERS; i++)
		pthread_mutex_init(&customersMutex[i], NULL);

	// init mutex for safe box
	pthread_mutex_init(&safeBoxMutex, NULL);

	// create thread for each transaction
	pthread_t threads[NUM_TRANSACTION];
	transaction_t transactions[NUM_TRANSACTION];

	for (int i = 0; i < NUM_TRANSACTION; i++)
	{
		transactions[i].amount = rand() % 100;
		transactions[i].customer_id = rand() % NUM_CUSTOMERS;

		if (rand() % 2 == 0)
			pthread_create(&threads[i], NULL, withdraw, &transactions[i]);
		else
			pthread_create(&threads[i], NULL, deposit, &transactions[i]);
	}

	// join all threads
	for (int i = 0; i < NUM_TRANSACTION; i++)
		pthread_join(threads[i], NULL);

	// destroy mutex for each account
	for (int i = 0; i < NUM_CUSTOMERS; i++)
		pthread_mutex_destroy(&customersMutex[i]);

	// destroy mutex for safe box
	pthread_mutex_destroy(&safeBoxMutex);

	return 0;
}