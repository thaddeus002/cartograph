/**
 * \file shape_data.h
 * lecture de données dans un fichier shp
 * auteur : Yannick
 * date : 26 juin 2008
 */


#ifndef SHAPE_DATA
#define SHAPE_DATA 1

#include <stdio.h>
#include <stdlib.h>
#include "shape_types.h"




/**************************************************/
/**            Lecture de l'entete              **/
/**************************************************/

/* renvoie un pointeur sur une entete ou NULL en cas d'erreur */
/* et affiche les infos à l'écran */
shape_header *lit_entete(FILE *fd);

/* Décripte les infos de l'entete */
int affiche_entete(shape_header *sh);

/* libère la mémoire */
int detruit_entete(shape_header *sh);



/**************************************************/
/**        Lecture des enregistrements           **/
/**************************************************/

/* liste chainée dont les  maillons contiennent des formes génériques */
typedef struct  Enregistrement_ {
    int numero;
    int longueur; /* en demi octets de la partie contenu de l'enregistrement */

    /* liste de structures ? */
    /* mais lesquelles ? */
    //Polygon *sh;
    GenericShape *sh;

    struct Enregistrement_ *prev;
    struct Enregistrement_ *next;

} Enregistrement;

/* decripte les infos concernant la forme générique fournie en argument */
int affiche_infos(GenericShape *p);

/* remplit la liste chainée en lisant un fichier de données */
//Enregistrement *lecture_fichier(char *fichier/* les données à lire */);
Enregistrement *lecture_fichier(FILE *fd, shape_header *ent);

/* libère la mémoire */
int detruit_enregistrement(Enregistrement *e);


#endif
