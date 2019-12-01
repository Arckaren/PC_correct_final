#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct mot{
    char *m;
    int disparu;
}mot;

typedef struct phraseM{
    mot* listeMots;
    int taille;
}phraseM;


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

char *affichage(phraseM p){
    char *temp= malloc(p.taille*sizeof(mot));
   
    for(int i = 0; i < p.taille; i++){
        if(p.listeMots[i].disparu == 0)
            strcat(temp, p.listeMots[i].m);
        else
            strcat(temp, "___");
        strcat(temp, " ");
    }

    return temp;
}

char *motAPlacer(phraseM p){
    char *temp= malloc(p.taille*sizeof(mot));
    for(int i = 0; i < p.taille; i++){
        if(p.listeMots[i].disparu == 1){
            strcat(temp, p.listeMots[i].m);
            if(i != p.taille-1)
                strcat(temp, ", ");
        }
    }
    return temp;
}

int trouRestant(phraseM p){
    int somme = 0;
    for(int i = 0 ; i < p.taille; i++){
        somme += p.listeMots[i].disparu;
    }
    return somme;
}

//Renvoie 0 si ca matche, 1 sinon
int bonMot(char *motEntre, char *motTableau){
    
    if (strcmp(motEntre, motTableau) != 0){
        return 1;
    }
    else {
        return 0;
    }
}

//fonction d'initialisation des structures

void initialisation(){
    
    char phrase[] = "Les mendiantes remunerees a hauteur de cinquante-sept euros par jour prenaient racine dans les quartiers résidentiels de Seoul";
    char delim[] = " ";

    int nbMot = nbWord(phrase);

    phraseM p;
    p.taille = nbMot;
    p.listeMots =malloc(nbMot*sizeof(mot));

    char *ptr = strtok(phrase, delim);

    p.listeMots[0].m = ptr;
    p.listeMots[0].disparu = 0;

    int i = 1;
    while(i!=nbMot)
	{
		ptr = strtok(NULL, delim);
        p.listeMots[i].m = ptr;
        p.listeMots[i].disparu = 0;
        i++;
    }

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
        p.listeMots[idMotEnleve[i]].disparu = 1;
    }

}

//TODO: main obsolète, à modifier
int main(int argc, char* argv[]){

    char phrase[] = "Les mendiantes remunerees a hauteur de cinquante-sept euros par jour prenaient racine dans les quartiers résidentiels de Seoul";
    char delim[] = " ";

    int nbMot = nbWord(phrase);

    phraseM p;
    p.taille = nbMot;
    p.listeMots =malloc(nbMot*sizeof(mot));

    char *ptr = strtok(phrase, delim);

    p.listeMots[0].m = ptr;
    p.listeMots[0].disparu = 0;

    int i = 1;
    while(i!=nbMot)
	{
		ptr = strtok(NULL, delim);
        p.listeMots[i].m = ptr;
        p.listeMots[i].disparu = 0;
        i++;
    }

    printf("%s\n", affichage(p));

    int nbMotEnleve = 5;

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

    for(int i = 0; i <nbMotEnleve; i++){
        p.listeMots[idMotEnleve[i]].disparu = 1;
    }

    // TODO: Vérifier que les mots sont corrects
   while(trouRestant(p) != 0){

        char motEcrit[BUFSIZ];
        char indiceEcrit[BUFSIZ];

        printf("%s\n", affichage(p));
        printf("Mots restants à placer : %s\n", motAPlacer(p));


        printf("Ecrire l'indice du mot : ");
        fgets(indiceEcrit, BUFSIZ, stdin);

        printf("Ecrire le mot : ");
        fgets(motEcrit, BUFSIZ, stdin);
        
        motEcrit[strlen(motEcrit)-1]='\0';
        int indice = atoi(indiceEcrit);
       
        if(bonMot(motEcrit, p.listeMots[idMotEnleve[indice-1]].m) == 0){
            p.listeMots[idMotEnleve[indice-1]].disparu = 0;
        }
   }

}