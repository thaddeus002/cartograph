/**
 * \file lecture_bln.h
 * \brief A BLN file contains informations to draw a line on a map.
 *
 * Each line of file contains the coordinates of a point. Linking the points
 * give a curve : country boundaries, rivers, roads, ...
 *
 * This module has a function to draw BLN data in a X11 window.
 */

#ifndef __BLN_H__
#define __BLN_H__ 1

#include "fenetre.h"
#include "bln.h"



/**
 * The drawing function.
 * 
 * Shows bln data in a X11 window.
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
