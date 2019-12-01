#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "structure.h"



/*void *envoi(void* e){

}*/
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
        p->listeMots[idMotEnleve[i]].disparu = 1;
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
    if((key=ftok("file.txt",0))==-1){
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

    //ecoute
    if(listen(ds, 7)==-1){
        printf("Impossible de connecter");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in client1, client2;

    socklen_t lg = sizeof(struct sockaddr_in);
    int dsc = accept(ds, (struct sockaddr*)&client1, &lg);

    if(dsc == -1){
        printf("Client impossible à connecter");
        exit(EXIT_FAILURE);
    }

    //while(1){
        printf("client 1 : %d\n", dsc);

        phraseM phr;
        initialisation(&phr);

        /*char const* tab[] = {"nane", "truc", "bidule"};
        make_phraseM(&phr, 3, tab);*/
        
        send_phraseM(&phr, dsc);


        destroy_phraseM(&phr);
    //}
    close(dsc);
    close(ds);


    return 0;
}