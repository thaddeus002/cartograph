/**
 * \file yXinterface.c
 * Creating an "yImage" from a Xlib Drawable.
 */

#include "yXinterface.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xutil.h>

char x_error;

static void __handle_x_error(Display * d, XErrorEvent * ev)
{
    d = NULL;
    ev = NULL;
    x_error = 1;
}


yImage *create_yImage_from_drawable(Drawable win, Visual *vis, Display  *disp, int x, int y, int width, int height)
{
    unsigned char      *data = NULL;
    /* xx and yy are counters */
    int                 xx, yy, w, h, inx, iny, clipx, clipy, rx, ry;
    XImage             *xim;

    /* attributes of "win" and of the root window */
    XWindowAttributes   xatt, ratt;
    XErrorHandler       erh = NULL;
    Window              chld;
    char                is_pixmap = 0;
    yImage              *im;

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

    /* copy content */
    data = malloc(width * height * 3);
    if (data)
    {
        unsigned char *ptr = data;
        switch (xatt.depth)
        {
        case 24:
        case 32:
            for (yy = 0; yy < height; yy++)
            {
                for (xx = 0; xx < width; xx++)
                {
                    unsigned long pixel = XGetPixel(xim, xx, yy);
                    *ptr++ = (pixel >> 16) & 0xff;
                    *ptr++ = (pixel >> 8) & 0xff;
                    *ptr++ = pixel & 0xff;
                }
            }
            break;
        default: /* not supported case */
            fprintf(stderr, "Image's depth not supported: %d\n", xatt.depth);
            free(data);
            data = NULL;
        }
    }

    XDestroyImage(xim);

    im = NULL;
    if (data)
    {
        int err;
        im = y_create_image(&err, data, width, height);
        free(data);
    }
    return(im);
}
