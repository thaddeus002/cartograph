/**
 * \file window.h
 * Management of a X11 window and the main functions of the xlib.
 */


#ifndef __WINDOW_H__
#define __WINDOW_H__ 1

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/xpm.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>


/* XSystem variables */


typedef enum {NOIR=0, BLANC, ROUGE, VERT, BLEU, ORANGE_, JAUNE, CYAN_, MAGENTA_, MARRON_} couleur;

//Pixel rouge, vert, bleu, orange, jaune, cyan, magenta; // red, green blue deja utilisé
Pixel pix_colore[10];


typedef struct {
    Display *dpy;
    Window win;
    Pixmap pixmap;
    GC gc;
    Visual *visual;
    /* window's size in pixels */
    int w, h;
} window_t;


/** Initialization of global variables. */
void init_Xvariable();


/* retourne w=0 si échec */
window_t create_window(int width, int height, int *depth);
/* retourne w=0 si echec */
window_t create_window_with_background_color(int width, int height, int *depth, couleur c);


/**
 * Manage mouse and keyboard events.
 * \param fen the window
 * \param event the event which occured
 */
int manage_event(window_t fen, XEvent event);


/* Drawing functions */

void do_output_pixel(window_t f, int u, int v, Pixel p);
void do_output(window_t f, int u, int v, couleur n);


/**
 * Draw lignes in window's coordinates.
 * \param f where to draw
 * \param points array of points defining a multiline curve
 * \param nb number of points in the array 'points'
 * \param n the lines' color
 * \param largeur lines' width
 */
void trace_lignes(window_t f, XPoint *points, int nb, couleur n, int largeur);



/**
 * Fill a polygon.
 * \param f where to draw
 * \param points the polygon's corners
 * \param nb number of points
 * \param n color for the inner polygon
 */
void remplit(window_t f, XPoint *points, int nb, couleur n);



/**
 * \brief Draw a text in a window.
 * \param f where to draw
 * \param x x coordinate of the beginning point
 * \param y y coodinate of the beginning point
 * \param text the text to print
 * \param color text's color
 */
void display_text(window_t f, int x, int y, char * text, couleur c);


/**
 * \brief Draw a text in a window in white.
 *
 * Is equivantent to : display_text(f,x,y,text,BLANC).
 * \param f where to draw
 * \param x x coordinate of the beginning point
 * \param y y coodinate of the beginning point
 * \param text the text to print
 */
void display_text_blanc(window_t f, int x, int y, char * text);


/**
 * Draw a point in window's coordinates.
 */
int trace_point(window_t f, int x, int y, couleur c);



/**
 * Update/refresh window's view.
 */
void actualise_fenetre(window_t fen);


/**
 * Closes the displays.
 */
int close_window(window_t fen);

/* Save the window's drawing in files */

void sauve_fenetre_jpeg(window_t *f, char *filename);
void sauve_fenetre_png(window_t *f, char *filename);
void sauve_fenetre_tiff(window_t *f, char *filename);

#endif
