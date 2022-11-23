default: td1 sema RDV1 RDV2 lecteur redacteur
td1: td1.c
	gcc -Wall -g td1.c -o td1
# td2: td2.c
# 	gcc -Wall -g td2.c -o td2
sema: sema.c
	gcc -Wall -g sema.c -o sema
RDV1: RDV1.c
	gcc -Wall -g RDV1.c -o RDV1
RDV2: RDV2.c
	gcc -Wall -g RDV2.c -o RDV2
lecteur: lecteur.c
	gcc -Wall -g lecteur.c -o lecteur
redacteur: redacteur.c
	gcc -Wall -g redacteur.c -o redacteur
clean:
	rm -f td1 td2 sema RDV