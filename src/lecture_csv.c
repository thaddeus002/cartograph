/**
 * \file lecture_csv.c
 * Functions for reading points files in CSV format.
 * The columns are : lambx, lamby, prefecture, departement number.
 */


#include "lecture_csv.h"
#include <string.h>


static int pointe_ville(fenetre fen, poste_t *ville, forme_t pointage, int largeur, couleur cpoint, couleur ctexte){
    int p; // pointing success

    p=pointe(fen, ville->X, ville->Y, largeur, cpoint, pointage);

    if(p==0) display_text(fen.window, xpoint(&fen, ville->X)+9, ypoint(&fen, ville->Y), ville->commune, ctexte);

    return p ;
}


int point_data(fenetre fen, poste_t *points, forme_t pointage, int largeur, couleur cpoint, couleur ctexte){
    poste_t *current = points;
    int err = 0;
    while(current != NULL) {
        err = pointe_ville(fen, current, pointage, largeur, cpoint, ctexte);
        current = current->next;
    }
    return err;
}
