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

    if(p==0) display_text(fen, xpoint(&fen, ville->X)+9, ypoint(&fen, ville->Y), ville->commune, ctexte);

    return(p);
}



int traite_csv(fenetre fen, char *fichier_csv, forme_t pointage, int largeur, couleur cpoint, couleur ctexte){
    FILE *fd;
    poste_t *enregistrement;
    int err;

    // open file
    fd=fopen(fichier_csv, "r");
    if(!fd) {
        fprintf(stderr, "Could not open file %s\n", fichier_csv);
        return(1);
    }

    // read header line
    char header[500];
    fscanf(fd, "%s\n", header);

    // read data
    enregistrement = lit_enregistrement_csv(fd, &err);
    while(!err){
        pointe_ville(fen, enregistrement, pointage, largeur, cpoint, ctexte);
        enregistrement = lit_enregistrement_csv(fd, &err);
    }

    // close file
    fclose(fd);
    return(0);
}
