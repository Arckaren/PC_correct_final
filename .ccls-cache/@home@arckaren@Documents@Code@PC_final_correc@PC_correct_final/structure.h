#include <stdio.h>
#include <stdbool.h>

typedef struct mot{
    char* m;
    bool disparu;
    int size;
} mot;

typedef struct phraseM{
    int taille;
    mot* listeMots;
}phraseM;

// make_mot(&m, "chaine de caractere");
void make_mot(mot* m, char const* str);
void destroy_mot(mot* m);
void send_mot(mot* m, int socket_dsc);
void receive_mot(mot* m, int socket_dsc);

void receive_phraseM(phraseM* ph, int socket_dsc);

void send_phraseM(phraseM* ph, int socket_dsc);
void destroy_phraseM(phraseM* ph);

bool get_visible(int indice ,  phraseM* ph_trouee);

void set_visible(int  indice , phraseM* ph_trouee, bool non_visibility);

void make_phraseM(phraseM* ph, int nb_mot, char const** words);
typedef struct memoire{
    int id;
    phraseM p;
} memoire;
