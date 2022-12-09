# Objectif du TP5

Nous allons reprendre le code du TP4 pour ajouter l'attribution de threads à des cores du processeur.

On va pouvoir autoriser un thread à se lancer sur un core du processeur plus qu'un autre grâce aux fonctions vues dans les 2 exercices précédents.

## Répartition des threads sur les cores

Pour répartir les threads sur les cores, on va utiliser la fonction `sched_setaffinity` qui permet de définir les cores sur lesquels un thread peut s'exécuter.

La logique de répartition des threads se fait en fonction du nombre de threads et du nombre de cores disponibles.

J'ai 4 cores disponibles sur mon processeur, je lance N threads et j'ai le thread du main, le thread des mults et le thread du add.

Je répartis les threads en fonction de leur type.

- Le thread du main s'exécute sur le core 0
- Le thread des mults s'exécute sur plusieurs cores
- Le thread du add s'exécute sur le core 1

Finalement, on répartit les threads en fonction de l'index et du nb de threads.

Par exemple, on peut définir le main et le add sur le core 0 et 1 pour qu'ils soient fixés.
Puis, pour les mults, on vient les répartir sur les cores. Par exemple, si on a 4 threads, on vient les répartir sur les 4 pour avoir une répartition équilibrée. (en plus du main et du add)

Pour l'utilisation des méthodes et fonctions dans le code, on peut utiliser une fonction directement dans le thread pour définir le core sur lequel il va s'exécuter.
Ou alors, on peut utiliser une fonction dans le main pour définir le core sur lequel le thread va s'exécuter.
On peut également utiliser la fonction `pthread_setaffinity_np` qui permet de définir le core sur lequel le thread va s'exécuter.

(à voir en fonction de mon type d'implémentation)
