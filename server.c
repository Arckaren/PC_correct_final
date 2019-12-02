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
    data *d = (data*)e;
    printf("Envoi des donnees au client\n");

    sendIdClient(d->mem->idClient, d->mem->idSock);
    send_phraseM(&d->mem->p, d->mem->idSock);
    //while(1){

    //}
}

void *reception(void* r){
    data *d = (data*)r;

    //while(1){
        char id[BUFSIZ];
        char word[BUFSIZ];
        if (recv(d->mem->idSock, id, sizeof(id), 0)==-1){
        //erreur
        }

        int idMot = atoi(id);

        if (recv(d->mem->idSock, word, sizeof(word), 0)==-1){
        //erreur
        }
        printf("Le client %d a saisi les info : %d, %s\n",d->mem->idClient,idMot , word);
    //}
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
  srand (time (NULL));

    /*char phrase1[] = "Les mendiantes remunerees a hauteur de cinquante-sept euros par jour prenaient racine dans les quartiers résidentiels de Seoul";
    char phrase2[] = "l'oie niche haut l'oie niche bas mais ou niche l'hibou l'hibou niche ni haut ni bas";
    char phrase3[] = "Quelles sont les prix les plus bas pour les articles vendus cette semaine pendant la black week";
    char phrase4[] = "Le COBOL est un vieux langage qui à été crée par une femme nomée Grace Hopper";
    char phrase5[] = "Les chaussettes de l'archie duchesse sont t'elles sèches archi sèches";
    char phrase6[] = "Cinq dilettantes blâment des fiscalistes en armures de combat";
    char phrase7[] = "Keny Arcana mendiera des draps en satin aux sociopathes à Kyoto";
    char phrase8[] = "Les sidérurgistes contribuent à proposer deux armes aux monstres";
    char phrase9[] = "Les martyrs ne se débarrassent jamais d'omelettes aux champignons";
    char phrase10[] = "Est-ce que le penseur grossissait";
    char* tab_phrase={&phrase1,&phrase2,&phrase3,&phrase4,&phrase5,&phrase6,&phrase7,&phrase8,&phrase9,&phrase10};

    int pointeur_phrase= rand() % 10 ;
    
    char phrase[] =*(tab_phrase[pointeur_phrase]);
    char delim[] = " ";*/
   
    
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
    if((key=ftok("file.txt",2))==-1){
        printf("Creation de cle impossible");
        exit(EXIT_FAILURE);
    }

    data d;
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

    mem->mutexMot = (pthread_mutex_t*)malloc((mem->p.taille/3) * sizeof(pthread_mutex_t));
    for(int k = 0; k < mem->p.taille/3; k++){
        pthread_mutex_init(&mem->mutexMot[k], NULL);
    }

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

        if(fork()){
            printf("client %d : \n", mem->idClient);
            mem->idClient = nbClient;
            nbClient++;
            mem->idSock = dsc;
            d.mem = mem;
            pthread_t thread_envoi, thread_recep;
            if(pthread_create(&thread_envoi, NULL, envoi, &d)==-1){
                printf("Erreur creation pthread envoi");
                exit(EXIT_FAILURE);
            }

            if(pthread_create(&thread_recep, NULL, reception, &d)==-1){
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