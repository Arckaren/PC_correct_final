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

int returnIdTabMutex(int i, protector* tab, int taille){
    for(int k =0 ; k < taille; k++){
        if(tab[k].idMot == i)
            return k;
    }
    return -1;
}
bool bonMot(char *motEntre, char *motTableau){
    if (strcmp(motEntre, motTableau) != 0){
        return false;
    }
    else {
        return true;
    }
}

void *envoiClient(void* e){
    data *d = (data*)e;
    printf("Envoi des donnees au client\n");

    envoi(d->idSock, &d->idClient, sizeof(d->idClient));
    send_phraseM(&d->mem->p, d->idSock);
    while(1){
        if(d->mem->change){
            bool c = true;
            envoi(d->idSock, &c, sizeof(c));
            envoi(d->idSock, &d->mem->id, sizeof(d->mem->id));
            d->mem->change = false;
        }
    }
    pthread_exit(NULL);
}

void *reception(void* r){
    data *d = (data*)r;

    while(1){
        int32_t id;
        char word[BUFSIZ];
        recevoir(d->idSock, &id, sizeof(id));
        printf("id recu : %d", id);
        int index = returnIdTabMutex(id, d->mem->tabMutex, d->mem->nbMotEnleve);
        if(index == -1){
            printf("Index inexistant");
        }
        printf("index : %d", index);
        pthread_mutex_lock(&d->mem->tabMutex[index].mutexMot);
        recevoir(d->idSock, &word, sizeof(word));
        if(bonMot(word, d->mem->p.listeMots[id].m)){
            d->mem->p.listeMots[d->mem->id].disparu = false;
            d->mem->id = id;
            d->mem->change = true;
        }
        printf("\n Le client %d a saisi les info : %d, %s\n",d->idClient, id, word);

        pthread_mutex_unlock(&d->mem->tabMutex[index].mutexMot);
    }
    pthread_exit(NULL);
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

    if(argc != 2){
        printf("Nombre d'argument pas bon");
        exit(EXIT_FAILURE);
    }

    key_t key;

    if((key=ftok("file.txt",0))==-1){
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

    int semaphores = semget(key,7,IPC_CREAT|0666);
    if( semaphores == -1){
        perror(" echec de creation de la memoire partagee");
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
    mem->nbMotEnleve = (mem->p.taille)/3;
    mem->tabMutex = (protector*)malloc((mem->p.taille/3) * sizeof(protector));
    for(int k = 0; k < mem->p.taille/3; k++){
        pthread_mutex_init(&mem->tabMutex[k].mutexMot, NULL);
    }
    int t = 0;
    for(int k = 0; k < mem->p.taille; k++){
        if(mem->p.listeMots[k].disparu == true){
            mem->tabMutex[t].idMot = k;
            t++;
        }
    }
    d.mem->change = false;

    //ecoute
    if(listen(ds, 7)==-1){
        printf("Impossible de connecter");
        exit(EXIT_FAILURE);
    }

    int nbClient = 1;

    d.mem = mem;
    do{
        struct sockaddr_in client;
        socklen_t lg = sizeof(struct sockaddr_in);
        int dsc = accept(ds, (struct sockaddr*)&client, &lg);
        if(dsc == -1){
            printf("Client impossible à connecter");
            exit(EXIT_FAILURE);
        }
        d.idClient = nbClient;

        nbClient++;
        d.idSock = dsc;
        if(fork()){
            printf("client %d : , socket : %d\n", d.idClient, dsc);

            pthread_t thread_envoi, thread_recep;

            if(pthread_create(&thread_envoi, NULL, envoiClient, &d)==-1){
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
    for(int i=0 ;i<7;i++){
        semctl(semaphores,i,IPC_RMID);
    }
    return 0;
}