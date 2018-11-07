/**
 * \file lecture_bln.h
 * \brief Un fichier bln est un fichier texte contenant les informations pour faire un tracé.
 * Chaque ligne du fichier contient les coordonnées d'un point. En reliant les points
 * on obtient un tracé : fleuve, côte, frontière, route, ...
 *
 * \deprecated this file is for use with xlib only. If possible use bln.h instead.
 */

#ifndef __BLN_H__
#define __BLN_H__ 1

#include "fenetre.h"
#include "coordonnees.h"
#include "bln.h"

/* types */

/**
 * Les extrêmes présents dans un fichier bln.
 */
typedef struct {
    float xmin, xmax, ymin, ymax;
    int resultat; //code de retour de la fonction de recherche des bornes
} bornes_bln;


/* function than may be deprecated */

/* passer un nom de fichier bln en paramètre et on s'occupe de tout */
/* retourne un code d'erreur non nul en cas d'échec */
int trace_bln_lignes(char *fichier, fenetre f, int largeur, couleur c, int remplir, couleur remplissage);

/* new functions */

/**
 * The drawing function.
 * 
 * Shows bln data in a window.
 * \param data the bln data to draw
 * \param f the window where draw
 * \param largeur lines' width
 * \param c lines' color
 * \param remplir 1 if the closed forms must be filled
 * \param remplissage filling color
 * \return 0 on success or an error code
 */
int bln_show_in_window(bln_data_t *data, fenetre f, int largeur, couleur c, int remplir, couleur remplissage);



#endif
