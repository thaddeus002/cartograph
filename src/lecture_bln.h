/**
 * \file lecture_bln.h
 * \brief Un fichier bln est un fichier texte contenant les informations pour faire un tracé.
 * Chaque ligne du fichier contient les coordonnées d'un point. En reliant les points
 * on obtient un tracé : fleuve, côte, frontière, route, ...
 */

#ifndef __BLN_H__
#define __BLN_H__ 1

#include "fenetre.h"
#include "coordonnees.h"

/* types */

/**
 * Les extrêmes présents dans un fichier bln.
 */
typedef struct {
    float xmin, xmax, ymin, ymax;
    int resultat; //code de retour de la fonction de recherche des bornes
} bornes_bln;


/* fonctions */

bornes_bln cherche_bornes_bln(char *file);


/* passer un nom de fichier bln en paramètre et on s'occupe de tout */
/* retourne un code d'erreur non nul en cas d'échec */
int trace_bln_points(char *fichier, fenetre f, int largeur, couleur c, int remplir, couleur remplissage);

/* passer un nom de fichier bln en paramètre et on s'occupe de tout */
/* retourne un code d'erreur non nul en cas d'échec */
int trace_bln_lignes(char *fichier, fenetre f, int largeur, couleur c, int remplir, couleur remplissage);

/* lorsque les donnees sont en lon et lat */
int trace_bln_geo(char *fichier, fenetre f, int largeur, couleur c, int remplir, couleur remplissage);


#endif
