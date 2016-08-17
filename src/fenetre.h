/**
 * \file fenetre.h
 * Gestion de la fenetre et des principales fcts de la xlib
 */


#ifndef __FENETRE_H__
#define __FENETRE_H__ 1

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h> /* fcts floor, et sqrt */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/xpm.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

#include "outils.h"


/* variables du XSystem */
Pixel *pix; // tous les pixels avec les couleurs utiles - utilisation de pix pour la grille de relief :
//affichage.c:82:                   pointe_pixel(f, xlamb, ylamb, /*0*/largeur, pix[n+12000], CARRE);

#define PLAGE 12000 /* nb max de pixels dans le tableau pix */

typedef enum{NOIR=0, BLANC, ROUGE, VERT, BLEU, ORANGE, JAUNE, CYAN, MAGENTA, MARRON} couleur;
//Pixel rouge, vert, bleu, orange, jaune, cyan, magenta; // red, green blue deja utilisé
Pixel pix_colore[10];


typedef struct {
    Display *dpy;
    Window win;
    GC gc;
    Visual *visual;

    float x1, x2, y1, y2; /* les coordonnées planes (Lambert ou autres) correspondant aux bornes de la fenetre */
    int w, h; /* taille de la fenetre en pixels */
} fenetre;


/* */
void init_Xvariable();

/* fct utilisée par "carte.c" */
void init_colors(char *range, int depth, int gamma_f, int hmax);


/* retourne w=0 si échec */
fenetre cree_fenetre(int width, int height, int *depth, float x1, float x2, float y1, float y2/*,  XGCValues *gcv*/);
/* retourne w=0 si echec */
fenetre cree_fenetre_coloree(int width, int height, int *depth, float x1, float x2, float y1, float y2, couleur c);


/* gère les évènements clavier et souris */
int manage_event(fenetre fen, XEvent event);


/* fonctions de tracé */
/* devraient fctionner en coordonnées lambert -> à modifier */
void do_output_pixel(fenetre f, int u, int v, Pixel p);
void do_output(fenetre f, int u, int v, couleur n);

/* fctionne en coordonnées Lambert */
void trace_ligne(fenetre f, float u1, float u2, float v1, float v2, couleur n, int largeur);

/* fctionnent en coordonnées fenetres */
void trace_lignes(fenetre f, XPoint *points, int nb, couleur n, int largeur);
/* remplissage d'un polygone */
void remplit(fenetre f, XPoint *points, int nb, couleur n);


/* écrit du texte dans la fenêtre */
void display_text(fenetre f, int x, int y, char * text, couleur c);
void display_text_blanc(fenetre f, int x, int y, char * text);



/*trace un point en coordonnées fenêtre */
int trace_point(fenetre f, int x, int y, couleur c);
/* trace un point en fct des coordonnées Lambert */
int pointe_pixel(fenetre f, float xLamb, float yLamb, int L, Pixel p, forme_t forme);
int pointe(fenetre f, float xLamb, float yLamb, int L, couleur c, forme_t forme);

void trace_echelle(fenetre fen, couleur cdessin, couleur ctexte);


/* affichage dans la fenetre des modifications faites sur le pixmap par les fcts précédentes */
void actualise_fenetre(fenetre fen);

/* fermeture des display */
int fermeture(fenetre fen);


/* enregistre le dessin d'une fenetre dans un fichier */
void sauve_fenetre_jpeg(fenetre *f, char *filename);
void sauve_fenetre_png(fenetre *f, char *filename);
void sauve_fenetre_tiff(fenetre *f, char *filename);

#endif
