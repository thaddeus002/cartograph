/** \file lecture_grille.h */

#ifndef __GRILLE_H__
#define __GRILLE_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coordinates.h"

/**
 * Data format types
 */
typedef enum {
    INCONNU=0,
    GRD, /**< ASCCI Grid = DSAA */
    GRI, /**< binary grid (integer on 4 bytes) = Grille */
    GR2, /**<  binary grid (integer on 2 bytes) = Grille  */
    ALT, /**< binaire (2 octets inversés) */
    AZ2  /**<  */
} format_t;


/* Le contenu du fichier grille */
typedef struct grille_ {
    /* ENTETE */
    int longueur; /* longueur de l'entete en octets */
    format_t format; /* format du fichier de donnnées */
    int width, height; /* largeur et hauteur de la grille */
    float x1,x2,y1,y2,hmin,hmax; /* bornes de la zone concernée */
    /* DONNEES */
    int *data; /* tableau double dimensions traité linéairement (1 dim) pour simplifier l'alloc dyn */
    geodetic_system_t type; /* what are the data coordinates ? */
} grille_t;

void info_grille(grille_t *grille);

/* lit et analyse l'entête du fichier de données */
grille_t *read_header_data(FILE *fd);

/* lit une chaine dans le fichier de données et renvoie le nb de caracteres lus */
/* ou -1 en cas d'échec */
//int read_data(FILE *fd, char *valeur, char *format);

/* liberation de la taille de donnees */
void destroy_grille(grille_t *g);


/* lecture des données */
/* float x1 à y2 : fenetre d'interception souhaitée */
grille_t *lit_grille(char *fichier,float x1,float x2,float y1,float y2);

grille_t *lit_grille_entiere(char *fichier);


/* Met la valeur n, en position i,j dans les données */
int setdata(grille_t *grille, int i, int j, int n);

/* récupère la valeur en position i,j dans les données
** renvoie zéro si la position n'existe pas */
int getdata(grille_t *grille, int i, int j);


/* transformation de grille LATLON en LAMBERT2E */
/** \deprecated convert raster data between geodetic systems will not be
 * possible in this project */
grille_t *transforme_vers_Lambert(grille_t *grille_latlon);


#endif
