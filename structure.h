#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct mot{
    char* m;
    bool disparu;
    int size;
} mot;

typedef struct phraseM{
    int taille;
    mot* listeMots;
}phraseM;

typedef struct memoire{
    int idClient;
    int idSock;
    phraseM p;
    pthread_mutex_t *mutexMot;
} memoire;

typedef struct data{
    memoire *mem;
    int id;
    bool change;
}data;


//Methodes mots
void make_mot(mot* m, char const* str);

void destroy_mot(mot* m);

void send_mot(mot* m, int socket_dsc);

void receive_mot(mot* m, int socket_dsc);


void sendIdMot(int i, int socket_dsc, bool change);

int receiveIdMot(int socket_dsc);

bool receiveMotBool(int socket_dsc);

//Methodes phrases
void make_phraseM(phraseM* ph, int nb_mot, char const** words);

void receive_phraseM(phraseM* ph, int socket_dsc);

void send_phraseM(phraseM* ph, int socket_dsc);

void destroy_phraseM(phraseM* ph);

bool get_visible(int indice ,  phraseM* ph_trouee);

void set_visible(int  indice , phraseM* ph_trouee, bool non_visibility);


//Client
void sendIdClient(int id, int socket_dsc);

int receiveIdClient(int socket_dsc);



