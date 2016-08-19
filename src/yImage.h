/**
 * \file yImage.h
 * \brief images representation and basic operations. Plus load or save actions.
 *
 * yImage : sauvegarder des images dans différents formats
 * fonctions inspirées de la bibliothèque Imlib 1.x
 * => yImage est sous licence GPL (GNU GENERAL PUBLIC LICENSE)
 */

#include <stdint.h>
#include <stdio.h>

#ifndef __Y_IMAGE_H_
#define __Y_IMAGE_H_


/* error code */
#define ERR_NULL_PALETTE -1
#define ERR_NULL_COLOR -2
#define ERR_BAD_INDEX -3
#define ERR_ALLOCATE_FAIL -4




/** \brief Represents a color */
typedef struct _yColor {
    unsigned char r, g, b; /**< Red, green and blue levels, between 0 and 255 */
    unsigned char alpha; /**< alpha 0 => transparency ; 255 => opacity */
} yColor;


/** \brief a colors palete for 8 bits color images */
typedef /*unsigned char*/uint8_t yColorPalette_t[256*3];


/**
 * \brief A point define a pixel position in the image
 */
typedef struct {
    int x;
    int y;
} yPoint;



/** \brief A raster image */
typedef struct {
    unsigned char *rgbData; /* tableau RVBRVGRVB... */
    unsigned char *alphaChanel; /* array of alpha (8bits) values */
    int rgbWidth, rgbHeight; /* largeur et hauteur */
    int presShapeColor; /* indicate if shape_color is use or not */
    /* available if alpha_chanel == NULL and presShapeColor != 0 */
    yColor shapeColor; /* couleur correspondant à un pixel transparent */
} yImage;



/************************************************************/
/*               CREATION / DESTRUCTION DES IMAGES          */
/************************************************************/

void init_palette(yColorPalette_t palette, const uint8_t *pal);

/**
 * Create an yImage without transparency.
 * \param rbg_data the background image. Background will be black if NULL
 */
yImage *create_yImage(int *err, const unsigned char *rgb_data, int width, int height);


/**
 * create an yImage with an uniform background color
 */
yImage *create_uniform_yImage(int *err, yColor *background, int width, int height);



/**
 * free memory
 */
void destroy_yImage(yImage *im);

/************************************************************/
/*               MANIPULATION DES IMAGES                    */
/************************************************************/

/** recupere une couleur sur la palette */
int y_get_color_index(yColor *color, yColorPalette_t palette, int index);

/** create a color */
void y_set_color(yColor *color, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void y_init_color(yColor *color, unsigned int rgba);

/** rend l'image transparente */
int transp(yImage *im);


void superpose_images(yImage *back, yImage *fore, int x, int y);



/************************************************************/
/*                   DRAWING FUNCTIONS                      */
/************************************************************/

/**
 * \brief Set a color on a pixel of the image.
 * \param im the image to modify
 * \param color the color for the pixel
 * \param x x coordinate of pixel : 0 is left border
 * \param y y coordinate of pixel : 0 is top border
 */
void yImage_set_pixel(yImage *im, yColor *color, int x, int y);


/**
 * \brief Draw a line between two pixels on the image.
 * \param im the image where to draw
 * \param color the color for the line
 * \param x1 x coordinate of first pixel : 0 is left border
 * \param y1 y coordinate of first pixel : 0 is top border
 * \param x2 x coordinate of second pixel
 * \param y2 y coordinate of second pixel
 */
void y_draw_line(yImage *im, yColor *color, int x1, int y1, int x2, int y2);


/**
 * \brief draw some lines by linking points.
 * \param im the image where to draw
 * \param color the color for the lines
 * \param points a points table
 * \param nbPoints size of the points table
 */
void y_draw_lines(yImage *im, yColor *color, yPoint *points, int nbPoints);





#endif
