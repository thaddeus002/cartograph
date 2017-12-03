/**
 * \file lecture_csv.h
 * \brief Read functions for "points CSV files".
 *
 * These text files contents x, y (may be lambert coordinates or lat lon),
 * name (of the city or whatever object is pointed), optionnal info
 * (department number, ...).
 */

#ifndef __CSV_H__
#define __CSV_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include "fenetre.h"

/**
 * A pointed objet
 */
typedef struct {
    char commune[250]; /**< object name */
    float X, Y; /**< coordinates */
    int departement; /** department number if it exists */
} poste;



/*
// renvoie NULL si pas de lecture
poste *lit_enregistrement_cvs(FILE *fd, poste *enregistrement);
*/


int traite_csv(fenetre fen, char *fichier_cvs, forme_t pointage, int largeur, couleur cpoint, couleur ctexte);


#endif
