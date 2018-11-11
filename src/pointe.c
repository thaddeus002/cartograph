/** \file pointe.c */

#include "map.h"
#include "points.h"
#include "yImage.h"
#include "yImage_io.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LARGEUR_MAX 1400
#define HAUTEUR_MAX 800

/* The usage function */
void usage(char *prog){
    fprintf(stderr,"Points some sites, found in a CSV file.\n");
    fprintf(stderr,"Usage : %s <pointage.csv>\n", prog);
    exit(1);
}


/**
 * As we don't know what is the data domain, we better choose image's
 * size at execution time.
 * \param bornes data's boundaries
 * \param width must be allocated
 * \param heigth must be allocated
 * \return 0 in case of success
 */
static int choose_image_size(data_boundaries_t *bornes, int *width, int *height) {

    float r; //zoom ratio

    *width=(bornes->xmax-bornes->xmin)*10;
    *height=(bornes->ymax-bornes->ymin)*10;

    if((*width<0)|| (*height<0)) {
        printf("Boundaries error\n");
        return(1);
    }

    if(*width>LARGEUR_MAX) {
        *height=*height*LARGEUR_MAX/(*width); // ratio conservation
        *width=LARGEUR_MAX;
    }

    if(*height>HAUTEUR_MAX){
        *width=*width*HAUTEUR_MAX/(*height); // ratio conservation
        *height=HAUTEUR_MAX;
    }

    // max size
    if((*width<LARGEUR_MAX)&&(*height<HAUTEUR_MAX)){
        if((float)(*width)/LARGEUR_MAX>(float)(*height)/HAUTEUR_MAX){
            r=(float)(*width)/LARGEUR_MAX;
            *width=LARGEUR_MAX;
            *height=*height/r;
        }
        else {
            r=(float)(*height)/HAUTEUR_MAX;
            *height=HAUTEUR_MAX;
            *width=*width/r;
        }
    }

    printf("image width : %d - height : %d \n", *width, *height);
    return 0;
}



/**
 * Main program
 */
int main(int argc, char **argv){

    data_boundaries_t bornes; // boundaries of data file
    int width, height; // window's size
    int err; // error code
    map_t *map; // where make the drawing
    yColor *red, *yellow;
    poste_t *cities; // the points to plot


    if(argc<2) usage(argv[0]);
    printf("File to points : %s\n", argv[1]);

    cities = read_points_file(argv[1]);
    err = find_data_boundaries(cities, &bornes);
    if(err) {
        fprintf(stderr,"Bad file format : %s\n", argv[1]);
        destroy_points_data(cities);
        exit(err);
    }

    fprintf(stdout, "boundaries found : %f,%f - %f,%f\n", bornes.xmin, bornes.xmax, bornes.ymin, bornes.ymax);
    err = choose_image_size(&bornes, &width, &height);
    if(err) {
        fprintf(stderr, "Exiting program\n");
        destroy_points_data(cities);
        return err;
    }

    /* Creating the map */
    map = map_init(EPSG_4326, bornes.ymin, bornes.xmin, bornes.ymax, bornes.xmax, width, height);

    red = y_color(RED);
    yellow = y_color(YELLOW);
    map_point(map, cities, ROUND, 3, red, yellow);
    destroy_points_data(cities);
    free(red);
    free(yellow);

    y_save_png(map->image, "point_map.png");
    map_destroy(map);

    return 0;
}

