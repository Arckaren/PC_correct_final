#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "structure.h"


void *envoi(void* e){
    memoire *mem = (memoire*)e;
    printf("Envoi des donnees au client\n");
    /*char buf[20]; // size max of int converted in chars
    snprintf(buf, 20, "%d", mem->idClient);
    // 1. sending the word size (w/o \0)
    if(send(mem->idSock, buf, sizeof(buf), 0)==-1){
        printf("Envoi de l'id client impossible");
        exit(EXIT_FAILURE);
    }*/
    sendIdClient(mem->idClient, mem->idSock);
    send_phraseM(&mem->p, mem->idSock);
}

void *reception(void* r){
    memoire *mem = (memoire*)r;
    char id[BUFSIZ];
    char word[BUFSIZ];
    if (recv(mem->idSock, id, sizeof(id), 0)==-1){
      //erreur
    }
    if (recv(mem->idSock, word, sizeof(word), 0)==-1){
      //erreur
    }
    printf("Le client a saisi les info : %d, %s\n", atoi(id), word);
}

int nbWord(char *str){
    int state = 0;  
    unsigned nb_mot = 0; 
    while (*str)  
    {  
        if (*str == ' ' || *str == '\n' || *str == '\t')  
            state = 0;  
        else if (state == 0)  
        {  
            state = 1;  
            ++nb_mot;  
        }  
        ++str;  
    }  
    return nb_mot;  
}
void initialisation(phraseM *p){

    /*
    char* tab_phrase[10];
    char phrase1[] = "Les mendiantes remunerees a hauteur de cinquante-sept euros par jour prenaient racine dans les quartiers résidentiels de Seoul";
    char phrase2[] = "l'oie niche haut l'oie niche bas mais ou niche l'hibou l'hibou niche ni haut ni bas";
    char phrase3[] = "Quelles sont les prix les plus bas pour les articles vendus cette semaine pendant la black week";
    char phrase4[] = "Le COBOL est un vieux langage qui à été crée par une femme nomée Grace Hopper";
    char phrase5[] = "Les chaussettes de l'archie duchesse sont t'elles sèches archi sèches";
    char phrase6[] = "Cinq dilettantes blâment des fiscalistes en armures de combat";
    char phrase7[] = "";
    char phrase8[] = "";
    char phrase9[] = "";
    char phrase10[] = "";
    
    */
    
    char phrase[] = "Les mendiantes remunerees a hauteur de cinquante-sept euros par jour prenaient racine dans les quartiers résidentiels de Seoul";
    char delim[] = " ";

    int nbMot = nbWord(phrase);

    /*p.taille = nbMot;
    p.listeMots =malloc(nbMot*sizeof(mot));*/

    char *ptr = strtok(phrase, delim);
    const char *tab[nbMot];
    tab[0]= ptr;

    int i = 1;
    while(i!=nbMot)
	{
		ptr = strtok(NULL, delim);
        tab[i] = ptr;
        i++;
    }
    make_phraseM(p, nbMot, tab);

    int nbMotEnleve = nbMot/3;

    int idMotEnleve[nbMotEnleve];
    srand(time(NULL));

    int k, j;

    //Creation du tableau d'indices
    for(k = 0; k<nbMotEnleve;k++){
        int temp = (rand()%nbMot);
        for(j = 0; j < k; j++){
            if(temp == idMotEnleve[j])
                break;
        }
        if(k == j)
            idMotEnleve[k] = temp;
        else
            k--;
    }

    for(int i=0; i<nbMotEnleve; i++){
        for(int j=0; j<nbMotEnleve-i-1; j++){
            if(idMotEnleve[j]>idMotEnleve[j+1]){
                int temp = idMotEnleve[j];
                idMotEnleve[j] = idMotEnleve[j+1];
                idMotEnleve[j+1] = temp;
            }
        }
    }

    //met a jour le statut des mots de la liste en mots enleves

    for(int i = 0; i <nbMotEnleve; i++){
        set_visible(idMotEnleve[i], p, true);
    }

}

int main(int argc, char* argv[]){

    //commande à garder : elle va servir maggle
        // getsockname()
    if(argc != 2){
        printf("Nombre d'argument pas bon");
        exit(EXIT_FAILURE);
    }

    key_t key;
    if((key=ftok("file.txt",1))==-1){
        printf("Creation de cle impossible");
        exit(EXIT_FAILURE);
    }

    memoire *mem;

    int memoire_partage;
    if((memoire_partage = shmget(key, sizeof(memoire), IPC_CREAT | 0666))==-1){
        printf("Creation de la memoire partagee impossible");
        exit(EXIT_FAILURE);
    }

    mem = (memoire*)shmat(memoire_partage, NULL, 0);
    
    if((void*)mem == (void*)-1){
        printf("Erreur attachement");
        exit(EXIT_FAILURE);
    }


    //creation socket
    int ds = socket(PF_INET, SOCK_STREAM,0);
    if(ds == -1){
        printf("Création de socket impossible");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in ad;
    //test

    socklen_t lgA = sizeof(struct sockaddr_in);

    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = INADDR_ANY;
    ad.sin_port = htons(atoi(argv[1]));


    //nommage socket
    if(bind(ds, (struct sockaddr*)&ad, sizeof(ad))==-1){
        printf("Renommage impossible");
        exit(EXIT_FAILURE);
    }

    //Initialisation de la phrase
    phraseM phr;
    printf("Initialisation de la phrase du jeu\n");
    initialisation(&phr);
    
    mem->p = phr;


    //ecoute
    if(listen(ds, 7)==-1){
        printf("Impossible de connecter");
        exit(EXIT_FAILURE);
    }
    int nbClient = 1;
    do{
        struct sockaddr_in client;
        socklen_t lg = sizeof(struct sockaddr_in);
        int dsc = accept(ds, (struct sockaddr*)&client, &lg);
        if(dsc == -1){
            printf("Client impossible à connecter");
            exit(EXIT_FAILURE);
        }

        mem->idClient = nbClient;
        nbClient++;
        mem->idSock = dsc;

        printf("client %d : \n", mem->idClient);

        if(fork()){
            pthread_t thread_envoi, thread_recep;
            if(pthread_create(&thread_envoi, NULL, envoi, mem)==-1){
                printf("Erreur creation pthread envoi");
                exit(EXIT_FAILURE);
            }

            if(pthread_create(&thread_recep, NULL, reception, mem)==-1){
                printf("Erreur creation pthread reception");
                exit(EXIT_FAILURE);
            }

            if(pthread_join(thread_envoi, NULL)==-1){
                printf("Erreur attente fin du thread envoi ");
                exit(EXIT_FAILURE);
            }

            if(pthread_join(thread_recep, NULL)==-1){
                printf("Erreur attente fin du thread reception ");
                exit(EXIT_FAILURE);
            }
        }
        
        close(dsc);
    }while(1);

    //Fin de l'execution, destruction
    destroy_phraseM(&phr);
    close(ds);

    return 0;
}