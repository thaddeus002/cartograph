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

#ifndef Y_IMAGE_H_
#define Y_IMAGE_H_


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


void yImage_set_pixel(yImage *im, yColor *color, int x, int y);


void superpose_images(yImage *back, yImage *fore, int x, int y);


#endif
