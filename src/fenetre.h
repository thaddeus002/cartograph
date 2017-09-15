/**
 * \file fenetre.h
 * Management a window and the main functions of the xlib.
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


/* XSystem variables */

/** Array of the pixels with usefull color. Is used in relief grid. */
Pixel *pix;

/** Max number of pixels in 'pix' array */
#define PLAGE 12000

typedef enum {NOIR=0, BLANC, ROUGE, VERT, BLEU, ORANGE, JAUNE, CYAN, MAGENTA, MARRON} couleur;

//Pixel rouge, vert, bleu, orange, jaune, cyan, magenta; // red, green blue deja utilisé
Pixel pix_colore[10];


typedef struct {
    Display *dpy;
    Window win;
    GC gc;
    Visual *visual;

    /* flat coordinates (may be Lambert) of the window's bounds */
    float x1, x2, y1, y2;
    /* window's size in pixels */
    int w, h;
} fenetre;


/* */
void init_Xvariable();

/* fct utilisée par "carte.c" */
void init_colors(char *range, int depth, int gamma_f, int hmax);


/* retourne w=0 si échec */
fenetre cree_fenetre(int width, int height, int *depth, float x1, float x2, float y1, float y2);
/* retourne w=0 si echec */
fenetre cree_fenetre_coloree(int width, int height, int *depth, float x1, float x2, float y1, float y2, couleur c);



/**
 * Manage mouse and keyboard events.
 * \param fen the window
 * \param event the event which occured
 */
int manage_event(fenetre fen, XEvent event);


/* Drawing functions */

/* devraient fctionner en coordonnées lambert -> à modifier */
void do_output_pixel(fenetre f, int u, int v, Pixel p);
void do_output(fenetre f, int u, int v, couleur n);

/* fctionne en coordonnées Lambert */
void trace_ligne(fenetre f, float u1, float u2, float v1, float v2, couleur n, int largeur);


/**
 * Draw lignes in window's coordinates.
 * \param f where to draw
 * \param points array of points defining a multiline curve
 * \param nb number of points in the array 'points'
 * \param n the lines' color
 * \param largeur lines' width
 */
void trace_lignes(fenetre f, XPoint *points, int nb, couleur n, int largeur);


/**
 * Fill a polygon.
 * \param f where to draw
 * \param points the polygon's corners
 * \param nb number of points
 * \param n color for the inner polygon
 */
void remplit(fenetre f, XPoint *points, int nb, couleur n);


/**
 * \brief Draw a text in a window.
 * \param f where to draw
 * \param x x coordinate of the beginning point
 * \param y y coodinate of the beginning point
 * \param text the text to print
 * \param color text's color
 */
void display_text(fenetre f, int x, int y, char * text, couleur c);


/**
 * \brief Draw a text in a window in white.
 *
 * Is equivantent to : display_text(f,x,y,text,BLANC).
 * \param f where to draw
 * \param x x coordinate of the beginning point
 * \param y y coodinate of the beginning point
 * \param text the text to print
 */
void display_text_blanc(fenetre f, int x, int y, char * text);


/**
 * Draw a point in window's coordinates.
 */
int trace_point(fenetre f, int x, int y, couleur c);


/**
 * Draw a point in Lambert's coordinates.
 */
int pointe_pixel(fenetre f, float xLamb, float yLamb, int L, Pixel p, forme_t forme);

int pointe(fenetre f, float xLamb, float yLamb, int L, couleur c, forme_t forme);

void trace_echelle(fenetre fen, couleur cdessin, couleur ctexte);


/**
 * Update/refresh window's view.
 */
void actualise_fenetre(fenetre fen);


/**
 * Closes the displays.
 */
int fermeture(fenetre fen);

/* Save the window's drawing in files */

void sauve_fenetre_jpeg(fenetre *f, char *filename);
void sauve_fenetre_png(fenetre *f, char *filename);
void sauve_fenetre_tiff(fenetre *f, char *filename);

#endif
