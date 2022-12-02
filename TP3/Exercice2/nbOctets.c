#include <stdio.h> /* nbOctets.c */
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "partage.h"

/* Decrire le handler de signal pour SIGUSR1 */
/* ========================================= */
void handler(int signum)
{
  printf("Signal recu : %d\n", signum);
}

/* Le main */
/* ======= */
int main(int argc, char **argv)
{
  pid_t pidWC;
  pid_t pidREAD;
  int status; /* Pour les waitpid                        */

  int tube[2];
  FILE *fIn; /* Pour faire un fdopen : int -> FILE *    */

  struct sigaction action;

  Zone z;
  int *ptDeb; /* Un pointeur (int*) sur la zone debut    */

  char *fileName = NULL;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s fileName\n", argv[0]);
    return 1;
  }

  fileName = argv[1];

  /* A cause de warnings lorsque le code n'est pas encore la ...*/

  (void)action;
  (void)fIn;
  (void)tube;
  (void)status;
  (void)pidREAD;
  (void)pidWC;

  /* Gestion des signaux */
  /* =================== */

  /* Preparation de la structure action pour recevoir le signal SIGUSR1 */

  /* action.sa_handler = ... */
  action.sa_handler = handler;

  /* Appel a l'appel systeme sigaction */

  if (sigaction(SIGUSR1, &action, NULL) == -1)
  {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  /* Creation de la zone de memoire partagee */
  /* ======================================= */
  creerZonePartagee(sizeof(int), &z);

  ptDeb = (int *)z.debut; /* *ptDeb <=> *((int*)z.debut) */

  /* Creation du tube */
  /* ================ */
  pipe(tube);

  /* Creation du processus qui fera le exec ...   */
  /* ============================================ */

  /* pidWC=... */
  pidWC = fork();

  if (pidWC == -1)
  {
    perror("pidWC");
    exit(EXIT_FAILURE);
  }

  /* Dans le processus cree :
     - Rediriger la sortie standard vers le tube
     - Fermer le(s) descripteur(s) inutile(s) a cet enfant
     - Recouvrir par la commande ``wc''
  */
  if (pidWC == 0)
  {
    dup2(tube[1], STDOUT_FILENO); // on redirige la sortie standard vers le tube
    close(tube[0]);               // on ferme la lecture du tube car non utilisée
    close(tube[1]);               // on ferme l'écriture du tube car redirigée
    execlp("wc", "wc", "-c", fileName, NULL);
    perror("execlp");
    exit(EXIT_FAILURE);
  }

  /* Creation du processus qui fera la lecture ...*/
  /* ============================================ */

  /* pidREAD=... */
  pidREAD = fork();

  if (pidREAD == -1)
  {
    perror("pidREAD");
    exit(EXIT_FAILURE);
  }

  /* Dans le processus cree :
     - Fermer le(s) descripteur(s) inutile(s) a cet enfant
     - Ouvrir un flux fIn sur la sortie du tube: fIn=fdopen(tube[0],"r");
     - Lire le resultat via le flux fIn et le mettre dans la memoire partagee
     - Fermer le flux fIn et le(s) descripteur(s) encore ouvert(s)
     - Attendre un peu pour que le pere puisse faire pause avant
     - Envoyer le signal SIGUSR1 au pere
  */
  if (pidREAD == 0)
  {
    close(tube[1]); // on ferme l'écriture du tube car non utilisée
    fIn = fdopen(tube[0], "r");
    fscanf(fIn, "%d", ptDeb);
    fclose(fIn);
    close(tube[0]); // on ferme la lecture du tube car non utilisée
    sleep(3);
    kill(getppid(), SIGUSR1);
    exit(EXIT_SUCCESS);
  }

  /* La suite du pere */
  /* ================ */

  /* Fermer les descripteurs de tube inutiles au pere */
  // On ferme tous les accès aux tubes car inutile pour le père.
  close(tube[0]);
  close(tube[1]);

  /* Attente d'un signal */
  pause();

  /* Recuperer le resultat dans la memoire partagee */
  printf("Nombre d'octets : %d\n", *ptDeb);

  /* Attendre le 1er enfant  */
  waitpid(pidWC, &status, 0);

  /* Attendre le 2eme enfant */
  waitpid(pidREAD, &status, 0);

  /* Supprimer la memoire partagee */
  supprimerZonePartagee(&z);

  return 0;
}
