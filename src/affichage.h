/** \file affichage.h */

#ifndef __AFFICHAGE_H__
#define __AFFICHAGE_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fenetre.h"
#include "lecture_grille.h"



/** Show the data on a window */
int affiche_grille_(fenetre f, grille_t *grille, char *palette, char *color_scheme, int depth, int hmax);


/**
 * Read grid data and show them on a window.
 */
int affiche_grille(fenetre f, char *fichier, char *palette, char *color_scheme, int depth, int hmax);



#endif

