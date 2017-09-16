/**
 * \file yXinterface.c
 * yXinterface is part of yImage under GNU GENERAL PUBLIC LICENSE.
 * Creating an "yImage" from a Xlib Drawable.
 */

#include "yXinterface.h"

char x_error;

static void __handle_x_error(Display * d, XErrorEvent * ev)
{
  d = NULL;
  ev = NULL;
  x_error = 1;
}


yImage *create_yImage_from_drawable(Drawable win, Visual *vis, Display  *disp, int x, int y, int width, int height)
{
  unsigned char      *data = NULL, *ptr, r, g, b;
  unsigned long       pixel;
  int                 xx, yy, w, h, inx, iny, clipx, clipy, rx, ry; /* xx et yy sont des compteurs */
  XImage             *xim;

  /* attributes of "win" and of the root window */
  XWindowAttributes   xatt, ratt;
  //Colormap            cmap;
  //static char         shm_checked = 0, shm = 1;
  XErrorHandler       erh = NULL;
  Window              chld;
  char                is_pixmap = 0;
  yImage         *im;
 // ImlibColor          ctab[256];

  inx = 0;
  iny = 0;
  w = width;
  h = height;

  XGrabServer(disp); /* X server freeze */
  erh = XSetErrorHandler((XErrorHandler) __handle_x_error);
  x_error = 0;
  XGetWindowAttributes(disp, win, &xatt);
  XFlush(disp);
  if (x_error)
    {
      x_error = 0;
      is_pixmap = 1;
      XGetGeometry(disp, win, &chld, &rx, &rx,
           (unsigned int *)&xatt.width, (unsigned int *)&xatt.height,
           (unsigned int *)&rx, (unsigned int *)&xatt.depth);
      XFlush(disp);
      if (x_error)
    {
      XUngrabServer(disp);
      XFlush(disp);
      XSetErrorHandler((XErrorHandler) erh);
      return NULL;
    }
    }
  XSetErrorHandler((XErrorHandler) erh);
  if (!is_pixmap)
    {
      XGetWindowAttributes(disp, xatt.root, &ratt);
      XTranslateCoordinates(disp, win, xatt.root, 0, 0, &rx, &ry, &chld);
      if ((xatt.map_state != IsViewable) &&
      (xatt.backing_store == NotUseful))
    {
      XUngrabServer(disp);
      XFlush(disp);
      return NULL;
    }
    }
  clipx = 0;
  clipy = 0;

  x = x - inx;
  y = y - iny;

  width = xatt.width - x;
  height = xatt.height - y;
  if (width > w)
    width = w;
  if (height > h)
    height = h;

  if (!is_pixmap)
    {
      if ((rx + x + width) > ratt.width)
    width = ratt.width - (rx + x);
      if ((ry + y + height) > ratt.height)
    height = ratt.height - (ry + y);
    }
  if (x < 0)
    {
      clipx = -x;
      width += x;
      x = 0;
    }
  if (y < 0)
    {
      clipy = -y;
      height += y;
      y = 0;
    }
  if (!is_pixmap)
    {
      if ((rx + x) < 0)
    {
      clipx -= (rx + x);
      width += (rx + x);
      x = -rx;
    }
      if ((ry + y) < 0)
    {
      clipy -= (ry + y);
      height += (ry + y);
      y = -ry;
    }
    }
  if ((width <= 0) || (height <= 0))
    {
      XUngrabServer(disp);
      XSync(disp, False);
      return NULL;
    }

    /* getting the image */
    xim = XGetImage(disp, win, x, y, width, height, 0xffffffff, ZPixmap);

  XUngrabServer(disp);
  XFlush(disp);

  /*if (xatt.depth == 1)
    {
      ctab[0].r = 255;
      ctab[0].g = 255;
      ctab[0].b = 255;
      ctab[1].r = 0;
      ctab[1].g = 0;
      ctab[1].b = 0;
    }
  else if (xatt.depth <= 8)
    {
      XColor              cols[256];

      if (!is_pixmap)
    {
      cmap = xatt.colormap;
      if (cmap == None)
        cmap = id->x.root_cmap;
    }
      else
    cmap = id->x.root_cmap;

      for (i = 0; i < (1 << id->x.depth); i++)
    {
      cols[i].pixel = i;
      cols[i].flags = DoRed | DoGreen | DoBlue;
    }
      XQueryColors(disp, cmap, cols, 1 << id->x.depth);
      for (i = 0; i < (1 << id->x.depth); i++)
    {
      ctab[i].r = cols[i].red >> 8;
      ctab[i].g = cols[i].green >> 8;
      ctab[i].b = cols[i].blue >> 8;
      ctab[i].pixel = cols[i].pixel;
    }
    }*/


  /* copy content */
  data = malloc(width * height * 3);
  if (data)
    {
      ptr = data;
      switch (xatt.depth)
    {

    case 24:
    case 32:
      for (yy = 0; yy < height; yy++)
        {
          for (xx = 0; xx < width; xx++)
        {
          pixel = XGetPixel(xim, xx, yy);
          r = (pixel >> 16) & 0xff;
          g = (pixel >> 8) & 0xff;
          b = pixel & 0xff;
          *ptr++ = r;
          *ptr++ = g;
          *ptr++ = b;
        }
        }
      break;
    default: /* not supported case */
      /*for (yy = 0; yy < height; yy++)
        {
          for (xx = 0; xx < width; xx++)
        {
          r = rand();
          g = rand();
          b = rand();
          *ptr++ = r;
          *ptr++ = g;
          *ptr++ = b;
        }
        }*/
        fprintf(stderr, "Profondeur d'image non supportée :%d\n", xatt.depth);
        free(data);
        XDestroyImage(xim);
        return(NULL);
      break;
    }
    }

    XDestroyImage(xim);

    if (data)
    {

        im = malloc(sizeof(yImage));
        if (!im)
            return NULL;
        im->rgbWidth = width;
        im->rgbHeight = height;
        im->rgbData = malloc(im->rgbWidth * im->rgbHeight * 3);
        if (!im->rgbData)
        {
            free(im);
            return NULL;
        }
        memcpy(im->rgbData, data, im->rgbWidth * im->rgbHeight * 3);

        im->shapeColor.r=-1;
        im->shapeColor.g=-1;
        im->shapeColor.b=-1;

        free(data);
        return(im);
    } else {
        return(NULL);
    }
}

