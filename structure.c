#include "structure.h"
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>


void make_mot(mot* m, char const* str) {
  m->size = strlen(str);
  m->disparu = false;
  m->m = (char*)malloc(sizeof(char) * (m->size + 1));
  strcpy(m->m, str);
}

void destroy_mot(mot* m) {
  free(m->m);
}

static size_t const INT_STR_SIZE = 20;

void send_mot(mot* m, int socket_dsc) {
  // 1. send the word size without including the \0
  // 2. send the chars(the word)
  // 3. other metadata
  //note : tout en caractères
  char buf[INT_STR_SIZE]; // size max of int converted in chars
  snprintf(buf, INT_STR_SIZE, "%d", m->size);
  // 1. sending the word size (w/o \0)
  send(socket_dsc, buf, sizeof(buf), 0);
  
  // 2. sending the chars(the word)
  send(socket_dsc, m->m, m->size, 0);

  // 3. sending the boolean (visibility)
  char buf2[INT_STR_SIZE]; // size max of int converted in chars
  snprintf(buf2, INT_STR_SIZE, "%d", m->disparu);
  send(socket_dsc, buf2, sizeof(buf), 0);
}

void receive_mot(mot* m, int socket_dsc) {
    char buf[INT_STR_SIZE];
    if (recv(socket_dsc, buf, sizeof(buf), 0)==-1){
        //erreur
    }
    m->size = atoi(buf);
    m->m = (char*)malloc(sizeof(char) * (m->size + 1));
    if (recv(socket_dsc, m->m, m->size, 0)==1){
        //erreur
    }
    char buf2[INT_STR_SIZE];
    if (recv(socket_dsc, buf2, sizeof(buf2), 0)==1){
        //erreur
    }
    m->disparu = atoi(buf2);
}
 
void make_phraseM(phraseM* ph, int nb_mot, char const** words){
  ph->listeMots = (mot*)malloc(nb_mot * sizeof(mot));
  ph->taille = nb_mot;
  for(int i = 0 ; i < nb_mot; i++){
    make_mot(&ph->listeMots[i], words[i]);
  }
}

void send_phraseM(phraseM* ph, int socket_dsc){

  char buf[INT_STR_SIZE]; // size max of int converted in chars
  snprintf(buf, INT_STR_SIZE, "%d", ph->taille);
  // 1. sending the word size (w/o \0)
  send(socket_dsc, buf, sizeof(buf), 0);
  for(int i = 0; i < ph->taille; i++){
    send_mot(&ph->listeMots[i], socket_dsc);
  }
}

void receive_phraseM(phraseM* ph, int socket_dsc){
  char buf[INT_STR_SIZE];
  if (recv(socket_dsc, buf, sizeof(buf), 0)==-1){
      //erreur
  }
  ph->taille = atoi(buf);
  ph->listeMots = (mot*) malloc(ph->taille* sizeof(mot));
  for(int i = 0; i < ph->taille; i++){
    receive_mot(&ph->listeMots[i], socket_dsc);
  }
}

void destroy_phraseM(phraseM* ph){
  for(int i = 0; i < ph->taille; i++){
    destroy_mot(&ph->listeMots[i]);
  }
  free(ph->listeMots);
}

void sendIdClient(int id, int socket_dsc){
  char buf[INT_STR_SIZE]; // size max of int converted in chars
  snprintf(buf, INT_STR_SIZE, "%d", id);
  // 1. sending the word size (w/o \0)
  send(socket_dsc, buf, sizeof(buf), 0);
}

int receiveIdClient(int socket_dsc){
  char buf[INT_STR_SIZE];
  if (recv(socket_dsc, buf, sizeof(buf), 0)==-1){
      //erreur
  }
  return atoi(buf);
}

void sendIdMot(int i, int socket_dsc, bool change){
  char buf[INT_STR_SIZE]; // size max of int converted in chars
  snprintf(buf, INT_STR_SIZE, "%d", i);
  // 1. sending the word size (w/o \0)
  send(socket_dsc, buf, sizeof(buf), 0);
  char buf2[INT_STR_SIZE]; // size max of int converted in chars
  snprintf(buf2, INT_STR_SIZE, "%d", change);
  // 1. sending the word size (w/o \0)
  send(socket_dsc, buf2, sizeof(buf2), 0);
}

int receiveIdMot(int socket_dsc){
  char buf[INT_STR_SIZE];
  if (recv(socket_dsc, buf, sizeof(buf), 0)==-1){
      //erreur
  }
  return atoi(buf);
}
bool receiveMotBool(int socket_dsc){
  char buf[INT_STR_SIZE];
  if (recv(socket_dsc, buf, sizeof(buf), 0)==-1){
      //erreur
  }
  return atoi(buf);
}


void set_visible(int  indice , phraseM* ph_trouee, bool non_visibility){
  if(non_visibility == true){
    ph_trouee->listeMots[indice].disparu = true;
  }
  else{ 
    ph_trouee->listeMots[indice].disparu = false;
  }
}

bool get_visible(int indice ,  phraseM* ph_trouee){
  return  ph_trouee->listeMots[indice].disparu;
}