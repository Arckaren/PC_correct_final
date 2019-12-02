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

void *actualisation(void* donnees){
    memoire *mem = (memoire*)donnees;
    mem->idClient = receiveIdClient(mem->idSock);
    receive_phraseM(&mem->p, mem->idSock);
 
    printf("Bienvenue client %d, debut du jeu\n", mem->idClient);
    printf("%s\n", affichage(mem->p));
    printf("%s\n", motAPlacer(mem->p));
    int i=0;
    while(1){
        i = receiveIdMot(mem->idSock);
        if(i!=0){
            bool change = receiveMotBool(mem->idSock);
            mem->p.listeMots[i].disparu = change;
            printf("%s\n", affichage(mem->p));
            printf("%s\n", motAPlacer(mem->p));
        }
        i = 0;
    }
    pthread_exit(NULL);
}

void *envoi(void* donnees){
    memoire *mem = (memoire*)donnees;
    char motEcrit[BUFSIZ];
    char indiceEcrit[BUFSIZ];

    printf("Ecrire l'indice du mot : ");
    fgets(indiceEcrit, BUFSIZ, stdin);
    send(mem->idSock, indiceEcrit, sizeof(indiceEcrit),0);

    printf("Ecrire le mot : ");
    fgets(motEcrit, BUFSIZ, stdin);
    motEcrit[strlen(motEcrit)-1]='\0';
    send(mem->idSock, motEcrit, sizeof(motEcrit),0);
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
    
    memoire mem;

    pthread_t env, recep;

    mem.idSock = dS;

    if(pthread_create(&recep, NULL, actualisation, &mem)==-1){
        printf("Erreur creation thread actualisation");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&env, NULL, envoi, &mem)==-1){
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

    destroy_phraseM(&mem.p);
    close(dS);
}