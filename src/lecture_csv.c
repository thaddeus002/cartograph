/**
 * \file lecture_csv.c
 * Functions for reading points files in CSV format.
 * The columns are : lambx, lamby, prefecture, departement number.
 */


#include "lecture_csv.h"
#include "outils.h"
#include <string.h>



static int pointe_ville(fenetre fen, poste *ville, forme_t pointage, int largeur, couleur cpoint, couleur ctexte){
    int p; // pointing success

    p=pointe(fen, ville->X, ville->Y, largeur, cpoint, pointage);

    if(p==0) display_text(fen, transforme_x(ville->X, fen.w, fen.x1, fen.x2)+9, transforme_y(ville->Y, fen.h, fen.y1, fen.y2), ville->commune, ctexte);

    return(p);
}



int traite_csv(fenetre fen, char *fichier_csv, forme_t pointage, int largeur, couleur cpoint, couleur ctexte){
    FILE *fd;
    char buf_read[300];
    int i,j; // nb de donnees lues, compteurs
    char com[250]; // nom de la commune
    //int d; // numero du departement
    int MAJ; //majuscule requise (traitement du nom de commune caract�re par caract�re)
    poste *enregistrement;

    enregistrement=malloc(sizeof(poste));


    //ouverture du fichier
    fd=fopen(fichier_csv, "r");
    if(!fd) {
        fprintf(stderr, "Ouverture impossible : %s\n", fichier_csv);
        return(1);
    }

    // on passe la premiere ligne
    if(fgets(buf_read, 255, fd)==NULL) {
        fprintf(stderr, "Fichier vide : %s\n", fichier_csv);
        fclose(fd);
        return(1);
    }

    //lecture des donn�es


    while(fgets(buf_read, 255, fd)!=NULL){

        //printf("Ligne : %s", buf_read);
        i=sscanf(buf_read, "%f,%f,%[a-zA-Z- \"\'],%d\n", &(enregistrement->X),&(enregistrement->Y), com, &(enregistrement->departement));

        if(i<3){
            fprintf(stderr,"\n%s : ligne incorrecte :\n%s\n\n", fichier_csv, buf_read);
            return(2);
        }
        //printf("X=%f; Y=%f; NOM=%s\n",enregistrement->X,enregistrement->Y,com);

        //traitement du nom de la commune
        //pas de guillemets et majuscules seulement en d�but de noms propres
        MAJ=1; //majuscule requise pour le premier caract�re
        j=0;
        //printf("Nom de la commune : %s\n",enregistrement->commune);
        for(i=0;i+j<=strlen(com)-1;i++){
            if(com[i+j]=='"') { j++; i--; continue; }  // suppression des guillemets
            enregistrement->commune[i]=com[i+j];
            if(MAJ){
                if((enregistrement->commune[i]>='a')&&(enregistrement->commune[i]<='z')) enregistrement->commune[i]+='A'-'a';
                MAJ=0;
            }
            else {
                if((enregistrement->commune[i]>='A')&&(enregistrement->commune[i]<='Z')) enregistrement->commune[i]+='a'-'A';
            }
            if((enregistrement->commune[i]=='-')||(enregistrement->commune[i]=='\'')||(enregistrement->commune[i]==' ')) MAJ=1;
        }
        enregistrement->commune[i]='\0';
        // fin de traitement

        pointe_ville(fen, enregistrement, pointage, largeur, cpoint, ctexte);
    }


    free(enregistrement);
    // fermeture du fichier
    fclose(fd);
    return(0);
}
