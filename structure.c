#include "structure.h"
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>


//==================================================================fonction d'envois des message ==============================

static size_t const INT_STR_SIZE = 20;

void envoi_mot(char *c, int socket_dsc){
  int r = 0;
  while(r < (int) strlen(c))
    r+= send(socket_dsc, c+r, (int)strlen(c)-r, 0);
}

void envoi_chiffre(int *i, int socket_dsc) {
  // 1. send int
  envoi_mot((char*)i, socket_dsc);
}


void envoi_bool(bool *b, int socket_dsc){
  int r = 0;
  while(r < (int) sizeof(bool))
    r+= send(socket_dsc, b+r, sizeof(bool)-r, 0);
}

//======fonctions d'envois général=======

void send_mot(mot* m, int socket_dsc) {

  //Envoi du buffer avec la taille du message
  envoi_chiffre(&m->size, socket_dsc);

  // 2. sending the chars(the word)
  envoi_mot(m->m, socket_dsc);

  // 3. sending the boolean (visibility)
  envoi_bool(&m->disparu, socket_dsc);
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

//================================================================================fonctions de reception===============




void receive_mot(mot* m, int socket_dsc) {
    int receive;

    unsigned long  r = 0;
    int retour = 0;
    while(r < sizeof(int)){
      retour = recv(socket_dsc, ((char*)&(receive))+r, sizeof(int)-r, 0);
      if(retour ==-1){
          //erreur
          perror(" ceci n'est pas un entier");
          return;
      }
      else if(retour == 0){
          printf("Cloture de socket");
          return;
      }
      else{
          r += retour;
      }
    }
    m->size = receive;
    printf("%d",m->size);
    m->m = (char*)malloc(sizeof(char) * (m->size + 1));


    r = 0;
    retour = 0;
    while(r < (unsigned int)m->size){
      retour = recv(socket_dsc, m->m+r, m->size-r, 0);
      if (retour==-1){
        perror("noob");
      }
      else if(retour == 0) {
        printf("Socket fermee");
      }
      else {
        r += retour;
      }
    }

    r = 0;
    retour = 0;
    while(r < sizeof(bool)){
      retour = recv(socket_dsc, ((char*)&m->disparu)+r, sizeof(bool)-r, 0);
      if (retour==-1){
          perror("not a bool");
      }
      else if(retour == 0){
          printf("Socket fermee222222");
      }
      else{
        r+= retour;  
      }      
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


//=======================================================================fonction d'envois des id =============================

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


//===================================================================fontions reception des id =============================

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