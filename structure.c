#include "structure.h"
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>


//==================================================================fonctions d'envois des messages ==============================


void envoi(int socket_dsc, void const* buf, ssize_t size){
  ssize_t r = 0;
  char* cbuf = (char *)buf;
  // strlen: count until \0
  strcat(cbuf, "\0");
  while(r < size) {
    // TODO: if ( == -1)
    r+= send(socket_dsc, cbuf + r, size - r, 0);
  }
}

//======fonctions d'envois général=======

void send_mot(mot* m, int socket_dsc) {

  //Envoi du buffer avec la taille du message
  envoi(socket_dsc, &m->size, sizeof(m->size));

  // 2. sending the chars(the word)
  envoi(socket_dsc, m->m, m->size+1);

  // 3. sending the boolean (visibility)
  envoi(socket_dsc, &m->disparu, sizeof(m->disparu));
}


void send_phraseM(phraseM* ph, int socket_dsc){

  envoi(socket_dsc, &ph->taille, sizeof(ph->taille));
  for(int i = 0; i < ph->taille; i++){
    send_mot(&ph->listeMots[i], socket_dsc);
  }
}

//================================================================================fonctions de reception===============

void recevoir(int socket_dsc, void const* buf, ssize_t size){
  ssize_t r = 0;
  char * cbuf = (char *)buf;
  while(r < size) {
    // TODO: if ( == -1) and (== 0)
    r+= recv(socket_dsc, cbuf + r, size - r, 0);
  }
}


void receive_mot(mot* m, int socket_dsc) {

    recevoir(socket_dsc, &m->size, sizeof(m->size));
    m->m = (char*)malloc(sizeof(char) * (m->size+1));
    recevoir(socket_dsc, m->m, m->size+1);

    recevoir(socket_dsc, &m->disparu, sizeof(m->disparu));

}

void receive_phraseM(phraseM* ph, int socket_dsc){

  recevoir(socket_dsc, &ph->taille, sizeof(ph->taille));
  ph->listeMots = (mot*) malloc(ph->taille* sizeof(mot));
  for(int i = 0; i < ph->taille; i++){
    receive_mot(&ph->listeMots[i], socket_dsc);
  }
}
 


 //=========================================================================== fonctions de manipulation des structures ========

void make_mot(mot* m, char const* str) {
  m->size = strlen(str);
  m->disparu = false;
  m->m = (char*)malloc(sizeof(char) * (m->size + 1));
  strcpy(m->m, str);
}

void destroy_mot(mot* m) {
  free(m->m);
}


void make_phraseM(phraseM* ph, int nb_mot, char const** words){
  ph->listeMots = (mot*)malloc(nb_mot * sizeof(mot));
  ph->taille = nb_mot;
  for(int i = 0 ; i < nb_mot; i++){
    make_mot(&ph->listeMots[i], words[i]);
  }
}


void destroy_phraseM(phraseM* ph){
  for(int i = 0; i < ph->taille; i++){
    destroy_mot(&ph->listeMots[i]);
  }
  free(ph->listeMots);
}

//========================================================================methodes pour le jeu =================================

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
