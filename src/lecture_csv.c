/**
 * \file lecture_csv.c
 * Functions for reading points files in CSV format.
 * The columns are : lambx, lamby, prefecture, departement number.
 */


#include "lecture_csv.h"
#include "outils.h"
#include <string.h>



static int pointe_ville(fenetre fen, poste_t *ville, forme_t pointage, int largeur, couleur cpoint, couleur ctexte){
    int p; // pointing success

    p=pointe(fen, ville->X, ville->Y, largeur, cpoint, pointage);

    if(p==0) display_text(fen, transforme_x(ville->X, fen.w, fen.x1, fen.x2)+9, transforme_y(ville->Y, fen.h, fen.y1, fen.y2), ville->commune, ctexte);

    return(p);
}



int traite_csv(fenetre fen, char *fichier_csv, forme_t pointage, int largeur, couleur cpoint, couleur ctexte){
    FILE *fd;
    char buf_read[300];
    poste_t *enregistrement;
    int err;

    // open file
    fd=fopen(fichier_csv, "r");
    if(!fd) {
        fprintf(stderr, "Ouverture impossible : %s\n", fichier_csv);
        return(1);
    }

    // read data
    enregistrement = lit_enregistrement_csv(fd, &err);
    while(!err){
        pointe_ville(fen, enregistrement, pointage, largeur, cpoint, ctexte);
    }

    // close file
    fclose(fd);
    return(0);
}
