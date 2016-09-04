/**
 * \file map.c
 *
 */

#include "map.h"
#include "yImage.h"
#include "outils.h"
#include "bln.h"
#include <stdlib.h>
#include <math.h>


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

    bln_data_t *data = bln_read_file(blnFile);
    int err = 0;

    if(data != NULL) {
        map_trace_bln_data(map,data,color);
        bln_destroy(data);
    } else {
        err = 1;
    }

    return err;
}



/**
 * Add lines or polygons to the map.
 */
int map_trace_bln_data(map_t *map, bln_data_t *blnData, yColor *color){

    /* variables */
    int i; /* numéro de ligne de coordonnées */
    yPoint *points; /* liste des points à relier */
    bln_data_t *current; /* to cross the data */

    /* initialisations */
    i=0;
    current=blnData;

    while(current!=NULL){

        points=malloc(current->nbPoints*sizeof(yPoint));

        for(i=0; i<current->nbPoints; i++) {

            points[i].x=transforme_x(current->x[i], map->image->rgbWidth, map->boundaries.lonMin, map->boundaries.lonMax);
            points[i].y=transforme_y(current->y[i], map->image->rgbHeight, map->boundaries.latMin, map->boundaries.latMax);

        }

        #ifdef DEBUG
        fprintf(stdout, "tracé pour %d points\n", current->nbPoints);
        #endif

        y_draw_lines(map->image, color, points, current->nbPoints);
        free(points);

        current = current->next;
    }

    return(0);
}


map_t *map_create_with_bln(char *blnFile, yColor *background, yColor *color, yProjection proj, int width, int height){

    bln_data_t *data = bln_read_file(blnFile);
    map_t *map = NULL;

    if(data != NULL) {
        map = map_init(proj, data->ymin, data->xmin, data->ymax, data->xmax, width, height);
        map_set_background(map, background);
        map_trace_bln_data(map,data,color);
        bln_destroy(data);
    }

    return map;
}

