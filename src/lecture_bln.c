/**
 * \file lecture_bln.c
 * Draw BLN data.
 */

#include "lecture_bln.h"
#include "coordinates.h"
#include <stdio.h>
#include <stdlib.h>



/**
 * The drawing function.
 * 
 * Shows bln data in a window.
 * \param data the bln data to draw
 * \param f the window where draw
 * \param largeur lines' width
 * \param c lines' color
 * \param ligne 1 if the points must be joined by lines
 * \param remplir 1 if the closed forms must be filled
 * \param remplissage filling color
 * \return 0 on success or an error code
 */
static int bln_show_in_window_lines(bln_data_t *data, fenetre f, int largeur, couleur c, int ligne, int remplir, couleur remplissage) {

    bln_data_t *current = data;
    while(current != NULL) {

        int n = 0; // point index
        float Xm,Ym; // where show the countries' names

        if(ligne == 0) {
            while(n<current->nbPoints) {
                pointe(f,current->x[n],current->y[n],largeur,c,CARRE);
                n++;
            }
        } else {
            if(remplir && current->x[0]==current->x[current->nbPoints-1] && current->y[0]==current->y[current->nbPoints-1] ) {
                window_fill_polygon(f, current->x, current->y, current->nbPoints, remplissage);
            }
            window_draw_lines(f, current->x, current->y, current->nbPoints, c, largeur);
        }

        Xm=0.5*(current->xmin+current->xmax); Ym=0.5*(current->ymin+current->ymax);

        if((current->name!=NULL)&&(current->nbPoints>=2)) {
            display_text(f,xpoint(&f, Xm)-(strlen(current->name))*4,ypoint(&f, Ym),current->name,JAUNE);
        }

        current = current->next; 
    }
    return 0;
}


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
int bln_show_in_window(bln_data_t *data, fenetre f, int largeur, couleur c, int remplir, couleur remplissage) {
    return bln_show_in_window_lines(data, f, largeur, c, 1, remplir, remplissage);
}

