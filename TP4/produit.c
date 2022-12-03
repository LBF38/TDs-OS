#include <pthread.h> /* produit.c */
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

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

/*********** Function ***********/

/**
 * @brief Initialize the pending multiplication
 *
 * Set all multiplication to 1
 *
 * @param prod
 */
void initPendingMult(Product *prod)
{
    size_t i;
    for (i = 0; i < prod->size; i++)
    {
        prod->pendingMult[i] = 1;
    }
}

/**
 * @brief Calculate the sum of the pending multiplication
 *
 * PendingMult is an array of int. Each element is set to 1 when the multiplication is pending.
 * When the multiplication is done, the element is set to 0.
 * Therefore, if all multiplication are done, the sum of the array is 0.
 *
 * @param prod
 * @return int
 */
int nbPendingMult(Product *prod)
{
    size_t i;
    int nb = 0;
    for (i = 0; i < prod->size; i++)
    {
        nb += prod->pendingMult[i];
    }
    return (nb);
}

/**
 * @brief Function to waste time
 *
 * This function is used to waste time in order to simulate a long
 * computation.
 *
 * @param ms
 */
void wasteTime(unsigned long ms)
{
    unsigned long t, t0;
    struct timeval tv;
    gettimeofday(&tv, (struct timezone *)0);
    t0 = tv.tv_sec * 1000LU + tv.tv_usec / 1000LU;
    do
    {
        gettimeofday(&tv, (struct timezone *)0);
        t = tv.tv_sec * 1000LU + tv.tv_usec / 1000LU;
    } while (t - t0 < ms);
}

/*****************************************************************************/
void *mult(void *data)
{
    size_t index;
    size_t iter;

    /*=>Recuperation de l'index, c'est a dire index = ... */
    index = *((size_t *)data);

    fprintf(stderr, "Begin mult(%ld)\n", index);
    /* Tant que toutes les iterations */
    for (iter = 0; iter < prod.nbIterations; iter++) /* n'ont pas eu lieu              */
    {
        /*=>Attendre l'autorisation de multiplication POUR UNE NOUVELLE ITERATION...*/
        /* On vient tester la variable de condition entre mutex et on attend tant que cela n'est pas bon. */
        pthread_mutex_lock(&prod.mutex);
        while (prod.state != STATE_MULT)
        {
            pthread_cond_wait(&prod.cond, &prod.mutex);
        }
        /* Le mutex est verrouillé une fois la condition remplie
        mais relaché sinon et le thread attend la vérification de la condition.  */

        fprintf(stderr, "--> mult(%ld)\n", index); /* La multiplication peut commencer */

        /*=>Effectuer la multiplication a l'index du thread courant... */
        prod.v3[index] = prod.v1[index] * prod.v2[index];

        wasteTime(200 + (rand() % 200)); /* Perte du temps avec wasteTime() */

        /* Affichage du calcul sur l'index */
        fprintf(stderr, "<-- mult(%ld) : %.3g*%.3g=%.3g\n", index, prod.v1[index], prod.v2[index], prod.v3[index]);

        /*=>Marquer la fin de la multiplication en cours... */
        prod.pendingMult[index] = 0;
        pthread_cond_signal(&prod.cond);
        pthread_mutex_unlock(&prod.mutex);

        /*=>Si c'est la derniere... */
        if (index == prod.nbIterations - 1)
        {
            /*=>Autoriser le demarrage de l'addition... */
            pthread_mutex_lock(&prod.mutex);
            if (nbPendingMult(&prod) == 0)
            {
                prod.state = STATE_ADD;
            }
            pthread_mutex_unlock(&prod.mutex);
        }
    }
    fprintf(stderr, "Quit mult(%ld)\n", index);
    return (data);
}

/*****************************************************************************/
void *add(void *data)
{
    size_t iter;
    fprintf(stderr, "Begin add()\n");
    /* Tant que toutes les iterations */
    for (iter = 0; iter < prod.nbIterations; iter++) /* n'ont pas eu lieu              */
    {
        size_t index;

        /*=>Attendre l'autorisation d'addition... */
        pthread_mutex_lock(&prod.mutex);
        while (prod.state != STATE_ADD)
        {
            pthread_cond_wait(&prod.cond, &prod.mutex);
        }

        /* L'addition peut commencer */
        fprintf(stderr, "--> add\n");

        /* Effectuer l'addition... */
        prod.result = 0.0;
        for (index = 0; index < prod.size; index++)
        {
            /*=>A faire... */
            prod.result += prod.v3[index];
        }

        wasteTime(100 + (rand() % 100)); /* Perdre du temps avec wasteTime() */

        fprintf(stderr, "<-- add\n");

        /*=>Autoriser le demarrage de l'affichage... */
        prod.state = STATE_PRINT;
        pthread_cond_signal(&prod.cond);
        pthread_mutex_unlock(&prod.mutex);
    }
    fprintf(stderr, "Quit add()\n");
    return (data);
}

/*****************************************************************************/
int main(int argc, char **argv)
{
    size_t i, iter;
    pthread_t *multTh;
    size_t *multData;
    pthread_t addTh;
    void *threadReturnValue;

    /* A cause de warnings lorsque le code n'est pas encore la...*/
    (void)addTh;
    (void)threadReturnValue;

    /* Lire le nombre d'iterations et la taille des vecteurs */
    if ((argc <= 2) ||
        (sscanf(argv[1], "%lu", &prod.nbIterations) != 1) ||
        (sscanf(argv[2], "%lu", &prod.size) != 1) ||
        ((int)prod.nbIterations <= 0) || ((int)prod.size <= 0))
    {
        fprintf(stderr, "usage: %s nbIterations vectorSize\n", argv[0]);
        return (EXIT_FAILURE);
    }

    /* Initialisations (Product, tableaux, generateur aleatoire,etc) */
    prod.state = STATE_WAIT;
    prod.pendingMult = (int *)malloc(prod.size * sizeof(int));
    initPendingMult(&prod);

    /*=>initialiser prod.mutex ... */
    pthread_mutex_init(&prod.mutex, NULL);

    /*=>initialiser prod.cond ...  */
    pthread_cond_init(&prod.cond, NULL);

    /* Allocation dynamique des 3 vecteurs v1, v2, v3 */
    prod.v1 = (double *)malloc(prod.size * sizeof(double));
    prod.v2 = (double *)malloc(prod.size * sizeof(double));
    prod.v3 = (double *)malloc(prod.size * sizeof(double));

    /* Allocation dynamique du tableau pour les threads multiplieurs */
    multTh = (pthread_t *)malloc(prod.size * sizeof(pthread_t));

    /* Allocation dynamique du tableau des MulData */
    multData = (size_t *)malloc(prod.size * sizeof(size_t));

    /* Initialisation du tableau des MulData */
    for (i = 0; i < prod.size; i++)
    {
        multData[i] = i;
    }

    /*=>Creer les threads de multiplication... */
    for (i = 0; i < prod.size; i++)
    {
        pthread_create(&multTh[i], NULL, mult, &multData[i]);
    }

    /*=>Creer le thread d'addition...          */
    pthread_create(&addTh, NULL, add, NULL);

    srand(time((time_t *)0)); /* Init du generateur de nombres aleatoires */

    /* Pour chacune des iterations a realiser, c'est a dire :
       Tant que toutes les itérations n'ont pas eu lieu              */
    for (iter = 0; iter < prod.nbIterations; iter++)
    {
        size_t j;

        /* Initialiser aleatoirement les deux vecteurs */
        for (j = 0; j < prod.size; j++)
        {
            prod.v1[j] = 10.0 * (0.5 - ((double)rand()) / ((double)RAND_MAX));
            prod.v2[j] = 10.0 * (0.5 - ((double)rand()) / ((double)RAND_MAX));
        }

        /*=>Autoriser le demarrage des multiplications pour une nouvelle iteration..*/
        pthread_mutex_lock(&prod.mutex);
        prod.state = STATE_MULT;
        pthread_mutex_unlock(&prod.mutex);

        /*=>Attendre l'autorisation d'affichage...*/
        pthread_mutex_lock(&prod.mutex);
        while (prod.state != STATE_PRINT)
        {
            pthread_cond_wait(&prod.cond, &prod.mutex);
        }

        /*=>Afficher le resultat de l'iteration courante...*/
        fprintf(stderr, "Resultat de l'iteration %d : %f\n", (int)iter, prod.result);
        pthread_cond_signal(&prod.cond);
        pthread_mutex_unlock(&prod.mutex);
    }

    /*=>Attendre la fin des threads de multiplication...*/
    for (i = 0; i < prod.size; i++)
    {
        pthread_join(multTh[i], &threadReturnValue);
    }

    /*=>Attendre la fin du thread d'addition...*/
    pthread_join(addTh, &threadReturnValue);

    /*=> detruire prod.mutex ... */
    pthread_mutex_destroy(&prod.mutex);

    /*=> detruire prod.cond ... */
    pthread_cond_destroy(&prod.cond);

    /* Detruire avec free ce qui a ete initialise avec malloc */
    free(prod.pendingMult);
    free(prod.v1);
    free(prod.v2);
    free(prod.v3);
    free(multTh);
    free(multData);
    return (EXIT_SUCCESS);
}