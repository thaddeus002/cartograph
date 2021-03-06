/**
 * \file fenetre.h
 * Management of a X11 window and the main functions of the xlib.
 */


#ifndef __FENETRE_H__
#define __FENETRE_H__ 1

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

#include "outils.h"
#include "window.h"
#include "coordinates.h"


typedef struct {
    window_t window;
    geodetic_system_t gs;
    /* flat coordinates of the window's bounds */
    float x1, x2, y1, y2;
} fenetre;



/* retourne w=0 si �chec */
fenetre cree_fenetre(int width, int height, int *depth, float x1, float x2, float y1, float y2);
/* retourne w=0 si echec */
fenetre cree_fenetre_coloree(int width, int height, int *depth, float x1, float x2, float y1, float y2, couleur c);



/**
 * Draw lines on a window.
 *
 * \param f the window where draw
 * \param x an array of X coordinates
 * \param y an array of Y coordinates
 * \param nb the number of points(x,y)
 * \param c lines' color
 * \param w lines' width
 * \return 0 on success or an error code
 */
int window_draw_lines(fenetre f, float *x, float *y, int nb, couleur c, int w);


/**
 * Draw a filled polygon on a window.
 *
 * \param f the window where draw
 * \param x an array of X coordinates
 * \param y an array of Y coordinates
 * \param nb the number of points(x,y)
 * \param c polygon's color
 * \return 0 on success or an error code
 */
int window_fill_polygon(fenetre f, float *x, float *y, int nb, couleur c);

/**
 * Draw a point in plane coordinates.
 */
int pointe_pixel(fenetre f, float x, float y, int L, Pixel p, forme_t forme);

int pointe(fenetre f, float x, float y, int L, couleur c, forme_t forme);

void trace_echelle(fenetre fen, couleur cdessin, couleur ctexte);

/**
 * display a text on the map centered on a given point.
 * \param f where display the text
 * \param X X plane coordinate
 * \param Y Y plane coordinate
 * \param text the text to display
 * \param c text's color
 */
void fenetre_display_centered_text(fenetre f, float X, float Y, char *text, couleur c);


/**
 * display a text to label an object on tha map.
 * \param f where display the text
 * \param object the coordinates of the object to label
 * \param text the text to display
 * \param c text's color
 */
void fenetre_display_label(fenetre f, coord_plane object, char *text, couleur c);


/**
 * Closes the displays.
 */
int fermeture(fenetre fen);


#endif
