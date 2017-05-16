## Compilation

	make file=...

L'argument suivant `file` doit être un fichier contenant une fonction `main`. Exemples :

	make file=print_and_give.c
	make file=prime.c
	make file=clock.c

## Exécution

	./ckpn

## Nettoyage

	make clean

## Interface

Pour utiliser un des réseaux de Kahn, définir une macroconstante `KPNLIB` valant `tkpn`, `ptkpn`, `pskpn`, `skpn` ou `nkpn` suivant l'interface choisie, puis inclure `kpn.h`.

Par la suite, les fonctions accessibles sont :

 - `kpn_init` : pour débuter une fonction appelée par `kpn_doco`. Non nécessaire sauf si l'interface choisie est `skpn`.
 - `kpn_end` : pour terminer une fonction appelée par `kpn_doco`. Non nécessaire sauf si l'interface choisie est `skpn`.
 - `kpn_give` : pour redonner la main au « processus » simulé suivant dans le cadre du multitâche coopératif. Non nécessaire sauf si l'interface choisie est `skpn`.
 - `kpn_put` : insère un élément dans une file de communication. Bloquant si la file est pleine (cas de `pskpn`).
 - `kpn_get` : récupère un élément dans une file de communication. Bloquant si la file est vide.
 - `kpn_new_channel` : crée une nouveau canal de communication.
 - `kpn_delete_channel` : libère la mémoire utilisée par un canal.
 - `kpn_doco` : exécute parallèlement plusieurs processus.

Certains types de données sont également mis à disposition pour l'interface :

 - `kpn_port_t` : port, extrémité d'un canal. Type abstrait, dépendant fortement de l'interface choisie.
 - `kpn_chan_t` : canal utilisé pour communiquer entre deux processus. Constitué de deux canaux, `in` et `out`.
 - `kpn_proc_t` : type de données représentant un processus. C'est un pointeur sur une fonction de `void*` dans `void*`.

Toutes les fonctions qui retournent un entier retournent 0 en cas de succès, -1 en cas d'erreur. Dans le cas où c'est une fonction en amont qui a échoué, `errno` est positionnée correctement. Les fonctions renvoyant des pointeurs vers des données renvoient `NULL` en cas d'échec.

## Rapport

Le présent projet est constitué de 5 implémentations des réseaux de Kahn en C99 POSIX, respectant dans la mesure du possible l'interface monadique initiale donnée en OCaml.
 
 - `tkpn` : implémentation fondée sur les threads. La communication s'effectue par des files non bornées et des mutex.
 - `ptkpn` : implémentatin fondée sur les processus, communiquant par tubes.
 - `pskpn` : implémentation fondée sur les processus, communiquant par mémoire partagée. Ici les files utilisées sont de taille bornée par `MAX_FIFO_SZ`, définie dans `pskpn.h`, et les chaque élément inséré dans la file doit être de taille en bytes inférieure à `MAX_DATA_SIZE`, constante définie dans `static_fifo.h`.
 - `nkpn` : implémentation fondée sur les sockets.
 - `skpn` : implémentation où le parallélisme est simulée à l'aide d'un multitâche coopératif implémenté par des sauts non-locaux.

De plus, 3 exemples utilisant ces interfaces sont présents :

 - `print_and_give` : programme de test créant `NB_PROCS` processus qui chacun incrémente un compteur, puis donnent la main à un processus suivant en lui envoyant ce compteur. Un ordre aléatoire des processus est défini auparavant. Les processus s'arrêtent lorsque `S_UPPER_BOUND` est atteint.
 - `prime` : génère les nombres premiers jusqu'à `UPPER_BOUND`.
 - `clock` : librement inspiré du projet de système digital. Affiche l'heure toutes les secondes.

À l'exception de l'implémentation de simulation et de l'utilisation de `pthread_yield` dans la première implémentation, tout est conforme au standard POSIX.

# Implémentation 1 : threads

On utilise la bibliothèque `pthread`. Un port est représenté par une file non bornée (implémentée sous la forme d'une liste simplement chaînée) ainsi que par un mutex garantissant l'exclusivité de l'accès à celle-ci.

# Implémentation 2 : processus + tubes

On utilise `fork` et `pipe`. Les données sont écrites avec `write` et lues avec `read`.

# Implémentation 3 : processus + mémoire partagée

On utilise `mmap` pour stocker une file de taille fixe (implémentée dans `static_fifo.c`, sous la forme d'un buffer circulaire). Un mutex avec l'attribut `PTHREAD_PROCESS_SHARED` garantit l'exclusivité de l'accès à la mémoire.

# Implémentation 4 : sockets

On communique localement avec des sockets nommées sous la forme socketn, où n est le premier emplacement libre dans le répertoire courant. L'option `SO_REUSEADDR` est utilisée au moment de la construction des canaux. `doco` est implémenté grâce à `fork` et `wait`.

# Implémentation 5 : simulation

**Attention :** plusieurs comportements indéterminés étant inévitables, cette implémentation est moins robuste que les précédentes et peut présenter des bugs inattendus. Globalement, on sort de POSIX. De plus, cette implémentation utilise des variables globales et n'est donc **pas réentrante**. En particulier, on ne peut pas créer du parallélisme récursivement. 

Pour simuler le parallélisme, on implémente une forme de multitâche coopératif. Pour cela, on emploie les fonctions standards `setjmp` et `longjmp` qui permettent respectivement de sauvegarder et de restaurer le contexte local. `skpn_init` initialise une variable `__skpn_cnt` locale à la fonction lui permettant de connaître son identifiant et ainsi le processus suivant à qui donner la main. On stocke globalement une liste doublement chaînée « en dur » contenant les identifiants des processus actifs (`skpn_prev`, `skpn_next`, `skpn_used`). Pour l'ordonnancement, `skpn_sign` varie entre +1 et -1, ce qui permet de faire des allers-retours de la gauche vers la droite, puis de la droite vers la gauche. `skpn_end` permet de signifier que le processus n'est plus actif, et `skpn_give` donne la main au processus suivant.

