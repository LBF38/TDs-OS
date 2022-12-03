# Notes sur le TP4

## Guide de démarrage

Pour l'utilisation de ce TP, veillez à bien cloner tout le dossier `TP4`. Il contient tous les fichiers nécessaires à la compilation du TP.

Ensuite, vous pouvez compiler le TP avec la commande `make` et le lancer avec la commande `./produit`. Cela vous affichera le guide d'utilisation de la commande.

Enfin, vous pouvez utiliser la commande `./produit` avec un flag pour débugger le programme et voir les étapes plus en détails.

Quelques exemples de commande :

- `./produit 2 3` pour lancer le programme sur 2 itérations pour des vecteurs de taille 3.
- `./produit 2 3 -d` pour lancer le même programme en mode debug.

## Explications du code et du principe du TP

### Résumé du sujet

Nous souhaitons réaliser un produit scalaire entre 2 vecteurs.
Pour cela, nous allons utiliser un thread par multiplication et un pour l'addition finale.
L'objectif du TP est de découvrir et manipuler l'utilisation de threads.

### Structure du code

Le code est divisé en 3 fichiers :

- `produit.c` : contient le code principal du programme.
- `produit.h` : contient les déclarations des fonctions et des variables globales.
- `produit` : qui est l'exécutable du programme.

Dans le programme `produit.c`, nous avons 3 fonctions principales :

- `main` : qui est la fonction principale du programme. Elle initialise les variables, lance les threads et attend la fin de leur exécution.
- `mult` : qui est la fonction qui sera lancée par les threads. Elle réalise la multiplication d'un élément du vecteur.
- `add` : qui est la fonction qui sera lancée par le thread d'addition. Elle réalise l'addition des résultats des multiplications.

### Fonctionnement du programme

Le programme se lance avec 2 arguments : le nombre d'itérations et la taille des vecteurs.
Il va ensuite générer 2 vecteurs de taille `size` et les remplir aléatoirement.
Puis, il va créer `N = size` threads pour réaliser les multiplications et un thread pour réaliser l'addition.
Enfin, il attend la fin de l'exécution de ces threads et affiche le résultat.

A chaque fois que l'on doit accéder à une donnée partagée qui sera modifiée par un thread, nous utilisons un mutex pour éviter les problèmes de concurrence.
De plus, les variables conditionnelles permettent d'attendre la fin de l'exécution d'un thread avant de passer à l'étape suivante.
Cela est notamment utile pour gérer les itérations.
