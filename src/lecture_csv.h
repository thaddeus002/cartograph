/**
 * \file lecture_csv.h
 * fonctions pour la lecture des fichiers postes au format texte CVS postes
 * lambx, lamby, prefecture (ou nom de la ville ou du lieu ou de l'objet pointé),
 * numero_departement ou autre info facultative
 */

#ifndef __CSV_H__
#define __CSV_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include "fenetre.h"

typedef struct {
    char commune[250];
/*
    int xLamb, yLamb; // l'int est plus pratique, mais...
    float lon, lat; // ...l'utilisation du float est plus rigoureuse
*/
    float X, Y;
    int departement;
} poste;


/*
// ouvre le fichier et prépare la lecture : NON
// renvoie NULL en cas d'échec
FILE *init_fichier_postes(FILE *fd, char *fichier_cvs);

// ferme le fichier
void close_fichier_postes(FILE *fd);

// renvoie NULL si pas de lecture
poste *lit_enregistrement_cvs(FILE *fd, poste *enregistrement);
*/


int traite_csv(fenetre fen, char *fichier_cvs, forme_t pointage, int largeur, couleur cpoint, couleur ctexte);


#endif
