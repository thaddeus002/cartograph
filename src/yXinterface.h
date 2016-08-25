/**
 * \file yXinterface.h
 * yXinterface fait partie de yImage
 * creation d'une yImage a partir de Drawable Xlib
 * fonctions inspirées de la bibliothèque Imlib 1.x
 *      => yImage est sous licence GPL (GNU GENERAL PUBLIC LICENSE)
 */

#ifndef YXINTERFACE_H_
#define YXINTERFACE_H_ 1

#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include "yImage.h"



/**
 * Création d'une yImage à partir d'un Drawable (Window ou Pixmap) Xlib
 * \param win est le Drawable
 * \param vis et disp sont les paramètres X liés à win
 * \param x et y représentent le point de départ dans win pour l'extration de l'image
 * \param height et width sont la taille désirée pour l'image à créer
 * Cette fonction a été simplifiée et ne prend pas en compte SHM (shm.h)
 * Elle se limite également aux profondeur d'affichage de 24 ou 32 :
 *      Imlib prend également en compte 0 à 8 et 15 et 16
 * \return NULL if it fail
 */
yImage *create_yImage_from_drawable(Drawable win, Visual *vis, Display  *disp, int x, int y, int width, int height);

#endif
