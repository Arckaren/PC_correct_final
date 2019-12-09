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

typedef struct protector{
    pthread_mutex_t mutexMot;
    int idMot;
}protector;

typedef struct memoire{
    phraseM p;
    protector *tabMutex;
    int nbMotEnleve;
    int id;
    bool change;
} memoire;


typedef struct data{
    memoire *mem;
    int idClient;
    int idSock;
}data;


//Methodes mots
void make_mot(mot* m, char const* str);

void destroy_mot(mot* m);

void send_mot(mot* m, int socket_dsc);

void receive_mot(mot* m, int socket_dsc);

void recevoir(int socket_dsc, void const* buf, ssize_t size);

void envoi(int socket_dsc, void const* buf, ssize_t size);


//Methodes phrases
void make_phraseM(phraseM* ph, int nb_mot, char const** words);

void receive_phraseM(phraseM* ph, int socket_dsc);

void send_phraseM(phraseM* ph, int socket_dsc);

void destroy_phraseM(phraseM* ph);

bool get_visible(int indice ,  phraseM* ph_trouee);

void set_visible(int  indice , phraseM* ph_trouee, bool non_visibility);




