/** \file lecture_bln.c */
/* fonctions de tracé de données de contours présentes dans un fichier au
** format BLN */


#include "lecture_bln.h"
#include "coordonnees.h"
#include "outils.h"
#include <stdio.h>
#include <stdlib.h>


#define LIGNE 250 /* longueur maximale des lignes du fichier, au dela on n'en tient pas compte */



/**
 * transform WGS84 geographical data to plane Extended Lambert II data.
 */
static void geo2Lamb(bln_data_t *data) {

    bln_data_t *current = data;

    while(current != NULL) {
        int n;
        float xmin, xmax, ymin, ymax;
        for(n=0; n<current->nbPoints; n++) {
            coord_geo geo;
            geo.lambda = current->x[n];
            geo.phi = current->y[n];
            geo.h = 0.f ;
            coord_lamb lamb = Wgs84geo_to_Lambert(geo);
            current->x[n] = lamb.X;
            current->y[n] = lamb.Y;
            // find the new extremun
            if(n==0) {
                xmin=lamb.X;
                xmax=lamb.X;
                ymin=lamb.Y;
                ymax=lamb.Y;
            } else {
                if(lamb.X < xmin) {
                    xmin = lamb.X;
                } else if(lamb.X > xmax) {
                    xmax = lamb.X;
                }
                if(lamb.Y < ymin) {
                    ymin = lamb.Y;
                } else if(lamb.Y > ymax) {
                    ymax = lamb.Y;
                }
            }
        }
        current->xmin = xmin;
        current->xmax = xmax;
        current->ymin = ymin;
        current->ymax = ymax;
        current = current->next; 
    }
}


/**
 * Internal transformation to have data in hectometers.
 */
static void lambInHm(bln_data_t *data) {

    bln_data_t *current = data;

    while(current != NULL) {
        int n;
        for(n=0; n<current->nbPoints; n++) {
            current->x[n] = current->x[n]/100;
            current->y[n] = current->y[n]/100;
        }
        current->xmin = current->xmin/100;
        current->xmax = current->xmax/100;
        current->ymin = current->ymin/100;
        current->ymax = current->ymax/100;
        current = current->next; 
    }
}



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




/**
 * The drawing function.
 * 
 * Shows the data of a bln file in a window.
 * \param filename BLN file name
 * \param f the window where draw
 * \param largeur lines' width
 * \param c lines' color
 * \param ligne 1 if the points must be joined by lines
 * \param remplir 1 if the closed forms must be filled
 * \param remplissage filling color
 * \return 0 on success or an error code
 */
static int trace_bln_V2(char *filename, fenetre f, int largeur, couleur c, int ligne, int geo, int remplir, couleur remplissage) {

    bln_data_t *data = bln_read_file(filename);
    
    if(data == NULL) {
        return 1;
    }

    if(geo) {
        geo2Lamb(data);
        lambInHm(data);
    }

    bln_show_in_window_lines(data, f, largeur, c, ligne, remplir, remplissage);

    bln_destroy(data);
    return 0;    
}



/* passer un nom de fichier bln en paramètre et on s'occupe de tout */
/* retourne un code d'erreur non nul en cas d'échec */
int trace_bln_points(char *fichier, fenetre f, int largeur, couleur c, int remplir, couleur remplissage){

    return(trace_bln_V2(fichier,f, largeur, c,0, 0, remplir, remplissage));
}

/* passer un nom de fichier bln en paramètre et on s'occupe de tout */
/* retourne un code d'erreur non nul en cas d'échec */
int trace_bln_lignes(char *fichier, fenetre f, int largeur, couleur c, int remplir, couleur remplissage){

    return(trace_bln_V2(fichier, f, largeur, c,1 , 0, remplir, remplissage));
}


/* passer un nom de fichier bln en paramètre et on s'occupe de tout */
/* retourne un code d'erreur non nul en cas d'échec */
/* le fichier d'entrée contient des coordonnées en lon et lat */
int trace_bln_geo(char *fichier, fenetre f, int largeur, couleur c, int remplir, couleur remplissage){
    return(trace_bln_V2(fichier, f, largeur, c,1 , 1, remplir, remplissage));
}

