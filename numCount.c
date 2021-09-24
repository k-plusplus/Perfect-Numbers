// Author: Keanan Gabertan

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_COUNT 4

pthread_mutex_t g_lock, c_lock;

// Global variables
unsigned int LIMIT = 0;
unsigned int counter = 2;
unsigned int perfect_count = 0;
unsigned int abundant_count = 0;
unsigned int deficient_count = 0;


void *findCount()
{
    unsigned int n = 0;

    pthread_mutex_lock(&g_lock);
    n = counter;
    counter++;
    pthread_mutex_unlock(&g_lock);

    while (n <= LIMIT)
    {
        unsigned int runSum = 1;
        // find proper divisors
        for (int i = 2; i * i <= n; ++i)
        {
            // i is a divisor of n
            if (!(n % i))
            {
                // add sum with i
                runSum += i;
                // add sum with n divided by i
                runSum += (n / i);
            }
            // idk whats going on here
            if ((i*i) == n)
            {
                // sub sum by i
                runSum -= i;
            }
        }

        if (runSum == n)
        {
            pthread_mutex_lock(&c_lock);
            perfect_count++;
            pthread_mutex_unlock(&c_lock);
        }
        else if (runSum > n)
        {
            pthread_mutex_lock(&c_lock);
            abundant_count++;
            pthread_mutex_unlock(&c_lock);
        }
        else if (runSum < n)
        {
            pthread_mutex_lock(&c_lock);
            deficient_count++;
            pthread_mutex_unlock(&c_lock);
        }
        // increment counter so we arent in inf loop
        pthread_mutex_lock(&g_lock);
        n = counter;
        counter++;
        pthread_mutex_unlock(&g_lock);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    unsigned int tc = 1;

    // Error checking in command line
    if (argc == 1)
    {
        printf("Usage: ./numCount <s|p> <limitValue>\n");
        exit(0);
    }
    else if (argc != 3)
    {
        printf("Error, invalid command line arguments.\n");
        exit(0);
    }

    // Verify and grab limit value
    for (unsigned int i = 0; i < strlen(argv[2]); ++i)
    {
        if (!isdigit(argv[2][i]))
        {
            printf("Error, invalid limit value.\n");
            exit(0);
        }
    }

    LIMIT = atoi(argv[2]);
    if (LIMIT < 100)
    {
        printf("Error, limit must be > 100.\n");
        exit(0);
    }

    // Verify if sequential or parallel
    if (!strcmp(argv[1], "s"))
    {
        tc = 1;
    }
    else if (!strcmp(argv[1], "p"))
    {
        tc = THREAD_COUNT;
    }
    else
    {
        printf("Error, invalid command line arguments.\n");
        exit(0);
    }

    printf("Count of Perfect, Abundant, and Deficient numbers from 1 to %d\n", LIMIT);
    printf("Please wait. Running...\n\n");

    // Create threadpool
    pthread_t t[THREAD_COUNT];
    // Initialize mutex locks
    pthread_mutex_init(&g_lock, NULL);
    pthread_mutex_init(&c_lock, NULL);

    // Start threads
    for (int i = 0; i < tc; ++i)
        pthread_create(&t[i], NULL, &findCount, NULL);

    for (int i = 0; i < tc; ++i)
        pthread_join(t[i], NULL);

    // Display results
    printf("Count of Perfect Numbers = %d\n", perfect_count);
    printf("Count of Abundant Numbers = %d\n", abundant_count);
    printf("Count of Deficient Numbers = %d\n", deficient_count);
    printf("Total = %d\n", perfect_count + abundant_count + deficient_count);

    return 0;
}