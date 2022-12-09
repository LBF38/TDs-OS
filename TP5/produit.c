/* Produit header for imports and constants */
#include "produit.h"

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

/**
 * @brief To print the pendingMult array for debug control
 *
 * @param prod
 */
void debugPrintPendingMult(Product prod)
{
    size_t i;
    for (i = 0; i < prod.size; i++)
    {
        fprintf(stderr, YELLOW "pendingMult[%ld] = %d\n" RESET, i, prod.pendingMult[i]);
    }
}

/**
 * @brief To print the vectors for debug control
 *
 * @param prod
 */
void debugPrintVectors(Product prod)
{
    size_t i;
    for (i = 0; i < prod.size; i++)
    {
        fprintf(stderr, YELLOW "v1[%ld] = %f\n" RESET, i, prod.v1[i]);
        fprintf(stderr, YELLOW "v2[%ld] = %f\n" RESET, i, prod.v2[i]);
        fprintf(stderr, YELLOW "v3[%ld] = %f\n" RESET, i, prod.v3[i]);
    }
}

/*****************************************************************************/

/**
 * @brief Function to multiply vectors - Thread function
 *
 * This function is executed by each thread.
 * It is used to make the multiplication.
 *
 * @param arg
 * @return void*
 */
void *mult(void *data)
{
    size_t index;
    size_t iter;

    /*=>Recuperation de l'index, c'est a dire index = ... */
    index = *((size_t *)data);

    fprintf(stderr, "Begin mult(%ld)\n", index);
    /* Tant que toutes les iterations n'ont pas eu lieu */
    for (iter = 0; iter < prod.nbIterations; iter++)
    {
        /*=>Attendre l'autorisation de multiplication POUR UNE NOUVELLE ITERATION...*/
        /* On vient tester la variable de condition entre mutex et on attend tant que cela n'est pas bon. */
        pthread_mutex_lock(&prod.mutex);
        while (prod.state != STATE_MULT || prod.pendingMult[index] != 1)
        {
            pthread_cond_wait(&prod.cond, &prod.mutex);
        }
        pthread_mutex_unlock(&prod.mutex);
        /* Le mutex est verrouillé une fois la condition remplie
        mais relaché sinon et le thread attend la vérification de la condition.  */

        /* La multiplication peut commencer */
        fprintf(stderr, "--> mult(%ld)\n", index);

        /*=>Effectuer la multiplication a l'index du thread courant... */
        prod.v3[index] = prod.v1[index] * prod.v2[index];

        /* Perte du temps avec wasteTime() */
        wasteTime(200 + (rand() % 200));

        /* Affichage du calcul sur l'index */
        fprintf(stderr, "<-- mult(%ld) : %.3g*%.3g=%.3g\n", index, prod.v1[index], prod.v2[index], prod.v3[index]);

        /*=>Marquer la fin de la multiplication en cours... */
        pthread_mutex_lock(&prod.mutex);
        prod.pendingMult[index] = 0;

        if (debug == 1)
        {
            fprintf(stderr, BLUE "mult(%ld) : pendingMult[%ld] = %d\n" RESET, index, index, prod.pendingMult[index]);
            debugPrintPendingMult(prod);
        }

        /*=>Si c'est la derniere... */
        /*=>Autoriser le demarrage de l'addition... */
        /* On vérifie que toutes les multiplications ont été effectuées */
        if (nbPendingMult(&prod) == 0)
        {
            prod.state = STATE_ADD;
        }

        pthread_cond_broadcast(&prod.cond);
        pthread_mutex_unlock(&prod.mutex);
    }
    fprintf(stderr, "Quit mult(%ld)\n", index);
    return (data);
}

/*****************************************************************************/

/**
 * @brief Function to add the vectors - Thread function
 *
 * @param data
 * @return void*
 */
void *add(void *data)
{
    size_t iter;
    fprintf(stderr, "Begin add()\n");
    /* Tant que toutes les iterations n'ont pas eu lieu */
    for (iter = 0; iter < prod.nbIterations; iter++)
    {
        size_t index;

        /*=>Attendre l'autorisation d'addition... */
        pthread_mutex_lock(&prod.mutex);
        while (prod.state != STATE_ADD)
        {
            pthread_cond_wait(&prod.cond, &prod.mutex);
        }
        pthread_mutex_unlock(&prod.mutex);

        /* L'addition peut commencer */
        fprintf(stderr, "--> add\n");

        /* Effectuer l'addition... */
        prod.result = 0.0;
        for (index = 0; index < prod.size; index++)
        {
            /*=>A faire... */
            prod.result += prod.v3[index];
        }

        /* Perdre du temps avec wasteTime() */
        wasteTime(100 + (rand() % 100));

        fprintf(stderr, "<-- add\n");

        /*=>Autoriser le demarrage de l'affichage... */
        pthread_mutex_lock(&prod.mutex);
        prod.state = STATE_PRINT;
        pthread_cond_broadcast(&prod.cond);
        pthread_mutex_unlock(&prod.mutex);
    }
    fprintf(stderr, "Quit add()\n");
    return (data);
}

/*****************************************************************************/

/**
 * @brief Main function for launching the program.
 *
 * @param argc
 * @param argv
 * @return int
 */
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
        fprintf(stderr, "Usage: %s nbIterations vectorSize [optional flag]\n", argv[0]);
        fprintf(stderr, "\nOption flags:\n-d\tdebug mode\n");
        return (EXIT_FAILURE);
    }

    /* For debug flag */
    if (argc > 3 && strcmp(argv[3], "-d") == 0)
    {
        debug = 1;
    }

    /* Initialisations (Product, tableaux, generateur aleatoire,etc) */
    prod.state = STATE_WAIT;
    prod.pendingMult = (int *)malloc(prod.size * sizeof(int));
    // initPendingMult(&prod); // Initialiser le tableau des multiplicateurs en attente

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
        fprintf(stderr, GREEN "\nIteration %ld : \n" RESET, iter);
        size_t j;

        /* Initialiser aleatoirement les deux vecteurs */
        for (j = 0; j < prod.size; j++)
        {
            prod.v1[j] = 10.0 * (0.5 - ((double)rand()) / ((double)RAND_MAX));
            prod.v2[j] = 10.0 * (0.5 - ((double)rand()) / ((double)RAND_MAX));
        }

        if (debug == 1)
            debugPrintVectors(prod);

        /*=>Autoriser le demarrage des multiplications pour une nouvelle iteration..*/
        pthread_mutex_lock(&prod.mutex);
        initPendingMult(&prod); // Initialiser le tableau des multiplicateurs en attente pr l'itération en cours
        prod.state = STATE_MULT;
        pthread_cond_broadcast(&prod.cond);
        pthread_mutex_unlock(&prod.mutex);

        /*=>Attendre l'autorisation d'affichage...*/
        pthread_mutex_lock(&prod.mutex);
        while (prod.state != STATE_PRINT)
        {
            pthread_cond_wait(&prod.cond, &prod.mutex);
        }

        /*=>Afficher le resultat de l'iteration courante...*/
        fprintf(stderr, GREEN "Resultat de l'iteration %d : %f\n" RESET, (int)iter, prod.result);
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