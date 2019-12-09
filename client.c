#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "structure.h"

pthread_mutex_t printout = PTHREAD_MUTEX_INITIALIZER;



char *affichage(phraseM p){
    char *temp= malloc(p.taille*sizeof(mot));
   
    for(int i = 0; i < p.taille; i++){
        if(p.listeMots[i].disparu == false)
            strcat(temp, p.listeMots[i].m);
        else
            strcat(temp, "___");
        strcat(temp, " ");
    }

    return temp;
}
char *motAPlacer(phraseM p){
    char *temp= malloc(p.taille*sizeof(mot));
    for(int i = 0; i < p.taille; i++){
        if(p.listeMots[i].disparu == 1){
            char buf[10];
            snprintf(buf, 10, "%d", i);
            strcat(temp, buf);
            strcat(temp, " : ");
            strcat(temp, p.listeMots[i].m);
            if(i != p.taille-1)
                strcat(temp, ", ");
        }
    }
    return temp;
}
void affichageDebutGame(data *d){
    
    recevoir(d->idSock, &(d->idClient), sizeof(d->idClient));
    receive_phraseM(&d->mem->p, d->idSock);

    int k;
    printf("\n      =================================================");
    for(k=0;k<2;k++){
        printf("\n    |                                                   |");
    }
    printf("\n    |       Bienvenue client %d, debut du jeu            |", d->idClient);
    for(k=0;k<2;k++){
        printf("\n    |                                                   |");
    }
    printf("\n      =================================================\n\n");
    printf(" |     %s\n\n", affichage(d->mem->p));
    printf(" |          %s\n\n", motAPlacer(d->mem->p));
}

void *actualisation(void* donnees){
    data *d = (data*)donnees;
    int i = -1;
    bool change = false;
    while(1){
        while(!change){
            recevoir(d->idSock, &change, sizeof(change));
        }
        recevoir(d->idSock, &i, sizeof(i));
        printf("i : %d\n", i);
        d->mem->p.listeMots[i].disparu = false;
       
        printf("%s\n", affichage(d->mem->p));
        printf("%s\n", motAPlacer(d->mem->p));

        change = false;
    }
    pthread_exit(NULL);
}

void *envoiServeur(void* donnees){
    data *d = (data*)donnees;
    char motEcrit[BUFSIZ];
    int32_t indiceEcrit;
    while(1){
        pthread_mutex_lock(&printout);
        printf("Ecrire l'indice du mot : ");
        fflush(stdout);
        scanf("%d ", &indiceEcrit);

        envoi(d->idSock, &indiceEcrit, sizeof(indiceEcrit));

        printf("Ecrire le mot : ");
        fflush(stdout);
        fgets(motEcrit, BUFSIZ, stdin);
        motEcrit[strlen(motEcrit)-1] = '\0';
        envoi(d->idSock, &motEcrit, sizeof(motEcrit));

        pthread_mutex_unlock(&printout);
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    
    if(argc != 3){
        printf("Nombre d'argument pas bon");
        exit(EXIT_FAILURE);
    }

    int dS = socket(PF_INET, SOCK_STREAM,0);
    if(dS == -1){
        printf("Création de socket impossible");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in aS;
    aS.sin_family = AF_INET;

    inet_pton(AF_INET, argv[1], &(aS.sin_addr));
    aS.sin_port = htons(atoi(argv[2]));
    socklen_t lgA = sizeof(struct sockaddr_in);

    if(connect(dS, (struct sockaddr *) &aS, lgA)==-1){
        printf("Connexion impossible à la socket");
        exit(EXIT_FAILURE);
    }
    data d;
    memoire *mem;
    mem = (memoire*)malloc(sizeof(memoire));

    pthread_t env, recep;
    d.mem = mem;
    d.idSock = dS;

    affichageDebutGame(&d);

    if(pthread_create(&recep, NULL, actualisation, &d)==-1){
        printf("Erreur creation thread actualisation");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&env, NULL, envoiServeur, &d)==-1){
        printf("Erreur creation thread envoi");
        exit(EXIT_FAILURE);
    }

    if(pthread_join(recep, NULL)==-1){
        printf("Erreur attente de fin du thread");
        exit(EXIT_FAILURE);
    }
    if(pthread_join(env, NULL)==-1){
        printf("Erreur attente de fin du thread");
        exit(EXIT_FAILURE);
    }

    destroy_phraseM(&d.mem->p);
    close(dS);
}