/** \file affichage.h */

#ifndef __AFFICHAGE_H__
#define __AFFICHAGE_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fenetre.h"
#include "lecture_grille.h"



/* Affichage des données dans une fenetre */
int affiche_grille_(fenetre f, grille_t *grille, char *palette);


/*
*
*   lecture en une seule fct
*   et affichage dans la fenetre f
*
*/
int affiche_grille(fenetre f, char *fichier, char *palette);



#endif

