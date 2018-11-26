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

        bln_data_t *line = bln_toGS(current, f.gs);
        if(line == NULL) goto completed;


        if(ligne == 0) {
            while(n<line->nbPoints) {
                pointe(f,line->x[n],line->y[n],largeur,c,CARRE);
                n++;
            }
        } else {
            if(remplir && line->x[0]==line->x[line->nbPoints-1] && line->y[0]==line->y[line->nbPoints-1] ) {
                window_fill_polygon(f, line->x, line->y, line->nbPoints, remplissage);
            }
            window_draw_lines(f, line->x, line->y, line->nbPoints, c, largeur);
        }

        Xm=0.5*(line->xmin+line->xmax); Ym=0.5*(line->ymin+line->ymax);

        if((line->name!=NULL)&&(line->nbPoints>=2)) {
            fenetre_display_centered_text(f, Xm, Ym, line->name, JAUNE);
        }

        bln_destroy(line);
        completed:
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

