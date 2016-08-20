/**
 * \file map.c
 *
 */

#include "map.h"
#include "yImage.h"
#include "outils.h"
#include <stdlib.h>
#include <string.h> // temporary for strlen()



int map_intercept(bbox_t *b1, bbox_t *b2){

    int inter = 1;

    // compare point NW of b1 with point SE of b2

    if( (b1->latMax <= b2->latMin) || (b1->lonMax <= b2->latMin) ) {
        inter = 0;
    }

    // compare point NW of b2 with point SE of b1
    if ( inter && ((b2->latMax <= b1->latMin) || (b2->lonMax <= b1->latMin)) ) {
        inter = 0;
    }

    return inter;
}



/**
 * Create a new map with a empty drawing.
 * \return a pointer to a newly allocated structure
 */
map_t *map_init(yProjection proj, float latMin, float lonMin, float latMax, float lonMax, int width, int height){

    int err;
    map_t *map = malloc(sizeof(map_t));

    map->proj = proj;

    map->boundaries.latMin = latMin;
    map->boundaries.latMax = latMax;
    map->boundaries.lonMin = lonMin;
    map->boundaries.lonMax = lonMax;

    map->image=create_yImage(&err, NULL, width, height);

    return map;
}


/**
 * Free the memory used by a map.
 */
void map_destroy(map_t *map){
    if(map!=NULL) {
        destroy_yImage(map->image);
        free(map);
    }
}


/**
 * Copy the image background on the map.
 * \return the pointer to the new image
 */
yImage *map_set_image(map_t *map, yImage *image){

    int err;
    yImage *img = create_yImage(&err, image->rgbData, image->rgbWidth, image->rgbHeight);

    if(map->image!=NULL) {
        destroy_yImage(map->image);
    }

    map->image = img;

    return img;
}


/**
 * Copy the color background on the map.
 * \return the pointer to the map image
 */
yImage *map_set_background(map_t *map, yColor *color){

    int err;
    yImage *img = create_uniform_yImage(&err, color, map->image->rgbWidth, map->image->rgbHeight);

    destroy_yImage(map->image);

    map->image = img;

    return img;
}



/**
 * Add lines or polygons to the map.
 */
int map_trace_bln(map_t *map, char *blnFile, yColor *color){

   /* variables */
    FILE *fd; /* Descripteur pour l'ouverture du fichier */
    char buf_read[250]; /* lecture d'une ligne */
    int N; /* nb de valeurs (de lignes) pour un trait */
    int i; /* numéro de ligne de coordonnées */

    char X[20], Y[20]; //lecture d'entier ou de reel selon les cas
    float a, b; // coordonnées d'un point
    int x, y; // transformés en coordonnées fenetres

    int k; char pays[100], region[100]; // infos facultatives
    pays[0]='\0';

    yPoint *points; /* liste des points à relier */
    yPoint coordonnees; /* lu sur une ligne */


    /* ouverture du fichier en lecture */
    fd=fopen(blnFile, "r");
    if(fd==NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", blnFile);
        return(1);
    }

    /* initialisations */
    a=0; b=0;
    i=0;


    /* Lecture ligne par ligne */
    while(fgets(buf_read, 250, fd)!=NULL){

        if(i==0) { /* ligne d'entete */
            sscanf(buf_read, "%d,%d,%[a-zA-Z'\" -],%s", &N, &k, pays, region);
            if (N==0) continue; /* i reste à 0 pour la lecture de la prochaine ligne */
        }

        else { /* ligne de donnée : coordonnée d'un point */

            if(i==1){
                points=malloc(N*sizeof(yPoint));
                if(points==NULL){
                    fprintf(stderr, "Allocation mémoire impossible\n");
                    return(3);
                }
            }


            /* lecture d'un enregistrement */
            if(sscanf(buf_read, "%[0-9.-],%[0-9.-]", X, Y)==0) return(2);
            a=atof(X);
            b=atof(Y);

            #ifdef DEBUG
            fprintf(stdout, "Point %d : %f, %f\n", i, a, b);
            #endif

            x=transforme_x(a, map->image->rgbWidth, map->boundaries.lonMin, map->boundaries.lonMax);
            y=transforme_y(b, map->image->rgbHeight, map->boundaries.latMin, map->boundaries.latMax);

            coordonnees.x=x; coordonnees.y=y;
            points[i-1]=coordonnees;
        }

        /* evolution des compteurs */
        if(i==N) {

            #ifdef DEBUG
            fprintf(stdout, "tracé pour %d points\n", N);
            #endif

            y_draw_lines(map->image, color, points, N);
            free(points);

            i=0;
        }
        else i++;

    } /* Arrivée à la fin du fichier */

    fclose(fd);
    //if (points!=NULL) free(points);
    return(0);



}



