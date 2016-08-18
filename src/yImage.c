/**
 * \file yImage.c : sauvegarder des images dans différents formats
 * fonctions inspirées de la bibliothèque Imlib 1.x
 *      => yImage est sous licence GPL (GNU GENERAL PUBLIC LICENSE)
 */


#include "yImage.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h> //memset()


/************************************************************/
/*               CREATION / DESTRUCTION DES IMAGES          */
/************************************************************/


void init_palette(yColorPalette_t palette, const /*unsigned char*/uint8_t *pal){
    int i, j; /* compteur */

    if(pal==NULL) return;

    for(i=0; i<=255; i++){

        for(j=0; j<=2; j++)
            palette[3*i+j]=pal[3*i+j]*4;

        //printf("%d : %d,%d,%d\n", i, palette[3*i], palette[3*i+1], palette[3*i+2]);
    }
    //memcpy(palette, pal, 256*3);
}


/* create an yImage without transparency */
yImage *create_yImage(int *err, const unsigned char *rgbData, int width, int height){

    yImage *im;

    im=(yImage *)malloc(sizeof(yImage));

    if (im==NULL) {
        *err=ERR_ALLOCATE_FAIL;
        return(NULL);
    }

    im->rgbData=(unsigned char *) malloc(3*width*height);
    if(im->rgbData==NULL) {
        *err=ERR_ALLOCATE_FAIL;
        free(im);
        return(NULL);
    }

    im->alphaChanel=(unsigned char *) malloc(width*height);
        if(im->alphaChanel==NULL) {
        *err=ERR_ALLOCATE_FAIL;
        free(im->rgbData);
        free(im);
        return(NULL);
    }

    memset(im->alphaChanel, 255, width*height);

    if(rgbData==NULL) {
        memset(im->rgbData, 0, 3*width*height);
    } else memcpy(im->rgbData, rgbData, 3*width*height);

    im->rgbHeight=height;
    im->rgbWidth=width;

    im->presShapeColor=0;

    im->shapeColor.r=0;
    im->shapeColor.g=0;
    im->shapeColor.b=0;

    *err=0;
    return(im);
}


yImage *create_uniform_yImage(int *err, yColor *background, int width, int height){

    yImage *img = create_yImage(err, NULL, width, height);
    int pix;

    for(pix=0; pix<width*height; pix++) {
        img->rgbData[0]=background->r;
        img->rgbData[1]=background->g;
        img->rgbData[2]=background->b;
    }

    memset(img->alphaChanel, background->alpha, width*height);

    return img;
}




/* libération de la memoire */
void destroy_yImage(yImage *im){
    if(im!=NULL){
        if(im->rgbData!=NULL) free(im->rgbData);
        free(im);
    }
}


/************************************************************/
/*               MANIPULATION DES IMAGES                    */
/************************************************************/

/* recupere une couleur sur la palette */
int y_get_color_index(yColor *color, yColorPalette_t palette, int index){
    if(color==NULL) return(ERR_NULL_COLOR);
    if(palette==NULL) return(ERR_NULL_PALETTE);

    if((index<0)||(index>255)) return(ERR_BAD_INDEX);

    color->r=palette[3*index];
    color->g=palette[3*index+1];
    color->b=palette[3*index+2];
    color->alpha=255;

    return(0);
}

/* create a color */
void y_set_color(yColor *color, unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    color->r=r;
    color->g=g;
    color->b=b;
    color->alpha=a;
}

void y_init_color(yColor *color, unsigned int rgba){
    color->r=rgba/(256*256*256);
    color->g=(rgba/(256*256))%256;
    color->b=(rgba/256)%(256*256);
    color->alpha=rgba%(256*256*256);
}




/* rend l'image transparente */
int transp(yImage *im){
    if(im==NULL) return -1;

    if(im->alphaChanel==NULL) im->alphaChanel=(unsigned char *)malloc(im->rgbWidth*im->rgbHeight);
    if(im->alphaChanel==NULL) return ERR_ALLOCATE_FAIL;

    memset(im->alphaChanel, 0, im->rgbWidth*im->rgbHeight);
    im->presShapeColor=0;

    return 0;
}


void yImage_set_pixel(yImage *im, yColor *color, int x, int y){
    int pos; /* position of the pixel (x,y) in the data array */

    if(im==NULL) return;
    if(color==NULL) return;
    if((x<0) || (y<0)) return;
    if((x>=im->rgbWidth) || (y>=im->rgbHeight) )return;

    pos = 3*y*im->rgbWidth + 3*x;

    im->rgbData[pos]=color->r;
    im->rgbData[pos+1]=color->g;
    im->rgbData[pos+2]=color->b;

    im->alphaChanel[y*im->rgbWidth + x]=color->alpha;
}



/* */
void superpose_images(yImage *back, yImage *fore, int x, int y){

  yColor composition;
  int i, j;

  for(i=0; i<fore->rgbWidth; i++)
    for(j=0; j<fore->rgbHeight; j++){

      int xb, yb;
      xb=i+x; yb=j+y;

      if((xb>=0) && (xb<back->rgbWidth) && (y>=0) && (yb<back->rgbHeight)){

        int ab= back->alphaChanel[xb+yb*back->rgbWidth];
        int rb= back->rgbData[3*(xb+yb*back->rgbWidth)];
        int gb= back->rgbData[3*(xb+yb*back->rgbWidth)+1];
        int bb= back->rgbData[3*(xb+yb*back->rgbWidth)+2];

        int af= fore->alphaChanel[i+j*fore->rgbWidth];
        int rf= fore->rgbData[3*(i+j*fore->rgbWidth)];
        int gf= fore->rgbData[3*(i+j*fore->rgbWidth)+1];
        int bf= fore->rgbData[3*(i+j*fore->rgbWidth)+2];

        /* TODO vraie superposition de couleurs */
        composition.r=((255-af)*rb + af*rf)/255;
        composition.b=((255-af)*bb + af*bf)/255;//af>0?bf:bb;
        composition.g=((255-af)*gb + af*gf)/255;//af>0?gf:gb;
        composition.alpha=ab+((255-ab)*af/255);

        yImage_set_pixel(back, &composition, xb, yb);
      }
  }
}

