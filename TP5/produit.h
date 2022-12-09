#include <pthread.h> /* produit.c */
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"
#define RESET "\x1B[0m"

/* Debug Flag */
int debug = 0;

/*********** Data Type ***********/

/**
 * @brief States for the operations
 *
 * Four states to know what is the current state of the operation
 * - STATE_WAIT
 * - STATE_MULT
 * - STATE_ADD
 * - STATE_PRINT
 *
 */
typedef enum
{
    STATE_WAIT,
    STATE_MULT,
    STATE_ADD,
    STATE_PRINT
} State;

/**
 * @brief Product structure
 *
 * Structure shared between the threads to make the operations
 * - state : current state of the operation
 * - pendingMult : pointer to an array of the state of multiplication of each thread (0/1)
 * - cond : condition variable to wait for the state change
 * - mutex : mutex to protect the structure
 * - nbIterations : number of iterations to do
 * - size : size of the array
 * - v1 : first vector
 * - v2 : second vector
 * - v3 : result vector (v1 * v2)
 * - result : result of the multiplication and addition
 */
typedef struct
{
    State state;
    int *pendingMult;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    size_t nbIterations;
    size_t size;
    double *v1;
    double *v2;
    double *v3;
    double result;
} Product;

/*********** Data structure ***********/

Product prod;
