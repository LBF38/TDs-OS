# default: td1 sema RDV1 RDV2 lecteur redacteur
# td1: TP1/td1.c
# 	gcc -Wall -g td1.c -o td1
# # td2: td2.c
# # 	gcc -Wall -g td2.c -o td2
# sema: TP2/sema.c
# 	gcc -Wall -g sema.c -o sema
# RDV1: TP2/RDV1.c
# 	gcc -Wall -g RDV1.c -o RDV1
# RDV2: TP2/RDV2.c
# 	gcc -Wall -g RDV2.c -o RDV2
# lecteur: TP2/lecteur.c
# 	gcc -Wall -g lecteur.c -o lecteur
# redacteur: TP2/redacteur.c
# 	gcc -Wall -g redacteur.c -o redacteur
# clean:
# 	rm -f td1 sema RDV1 RDV2 lecteur redacteur

# default: TP1
TP1: TP1/makefile
	cd TP1; make
TP2: TP2/makefile
	cd TP2; make
TP3: TP3/makefile
	cd TP3; make
