/* lecture_grille.h */

#ifndef __GRILLE_H__
#define __GRILLE_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Type de format des donn�es */
typedef enum{INCONNU=0, GRD, GRI, GR2, ALT, AZ2} format_t;
/* GRD : Grille ASCII = DSAA
   GR2 : La m�me en binaire (entiers sur 2 octects) = Grill2
   GRI : binaire avec des entiers sur 4 octects = Grille
   ALT : binaire (2 octets invers�s)
   AZ2 : 
*/

/* type de coordonn�es */
typedef enum{INDEFINI=0, LATLON, LAMBERT2E, LAMBERT93} type_coordonnees;

/* Le contenu du fichier grille */
typedef struct grille_ {
	/* ENTETE */
	int longueur; /* longueur de l'entete en octets */
	format_t format; /* format du fichier de donnn�es */
	int width, height; /* largeur et hauteur de la grille */
	float x1,x2,y1,y2,hmin,hmax; /* bornes de la zone concern�e */
	/* DONNEES */
	int *data; /* tableau double dimensions trait� lin�airement (1 dim) pour simplifier l'alloc dyn */
	type_coordonnees type; /* � quoi correspondent les coordonn�es i,j du fichier? */
} grille_t;

void info_grille(grille_t *grille);

/* lit et analyse l'ent�te du fichier de donn�es */
grille_t *read_header_data(FILE *fd);

/* lit une chaine dans le fichier de donn�es et renvoie le nb de caracteres lus */
/* ou -1 en cas d'�chec */
//int read_data(FILE *fd, char *valeur, char *format);

/* liberation de la taille de donnees */
void destroy_grille(grille_t *g);


/* lecture des donn�es */
/* float x1 � y2 : fenetre d'interception souhait�e */
grille_t *lit_grille(char *fichier,float x1,float x2,float y1,float y2);

grille_t *lit_grille_entiere(char *fichier);


/* Met la valeur n, en position i,j dans les donn�es */
int setdata(grille_t *grille, int i, int j, int n);

/* r�cup�re la valeur en position i,j dans les donn�es 
** renvoie z�ro si la position n'existe pas */
int getdata(grille_t *grille, int i, int j);


/* transformation de grille LATLON en LAMBERT2E */
grille_t *transforme_vers_Lambert(grille_t *grille_latlon);


#endif
