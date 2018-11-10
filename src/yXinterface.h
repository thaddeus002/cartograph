/**
 * \file yXinterface.h
 * Creating an "yImage" from a Xlib Drawable.
 */

#ifndef YXINTERFACE_H_
#define YXINTERFACE_H_ 1

#include <X11/Xlib.h>
#include "yImage.h"



/**
 * Create an "yImage" from a Xlib Drawable (Window ou Pixmap).
 *
 * Only support 24 or 32 as image's depth.
 * 
 * \param win the Drawable
 * \param vis X Visual linked to "win"
 * \param disp X Display linked to "win"
 * \param x horizontal coordinate of the beginning point for image extraction
 * \param y vertical coordinate of the beginning point for image extraction
 * \param height height of the image to extract
 * \param width width of the image to extract
 * \return NULL if it fail
 */
yImage *create_yImage_from_drawable(Drawable win, Visual *vis, Display  *disp, int x, int y, int width, int height);

#endif
