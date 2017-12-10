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

/* Main program */
int main(int argc, char **argv){

    data_boundaries_t bornes; // boundaries of data file
    int width, height; // taille de la fenetre
    map_t *map; // where make the drawing
    float r; //ratio pour l'agrandissement
    yColor *red, *yellow;
    poste_t *cities; // the points to plot
    int err;


    if(argc<2) usage(argv[0]);
    printf("File to points : %s\n", argv[1]);

    cities = read_points_file(argv[1]);
    err = find_data_boundaries(cities, &bornes);
    if(err) {
        fprintf(stderr,"Bad file format : %s\n", argv[1]);
        exit(err);
    }

    fprintf(stdout, "found boundaries : %f,%f - %f,%f\n", bornes.xmin, bornes.xmax, bornes.ymin, bornes.ymax);
    width=(bornes.xmax-bornes.xmin)*10;
    height=(bornes.ymax-bornes.ymin)*10;

    if((width<0)|| (height<0)) {
        printf("Boundaries Error... Exiting program\n");
        exit(1);
    }

    if(width>LARGEUR_MAX) {
        height=height*LARGEUR_MAX/width; //conservation du ratio
        width=LARGEUR_MAX;
    }
    if(height>HAUTEUR_MAX){
        width=width*HAUTEUR_MAX/height; //conservation du ratio
        height=HAUTEUR_MAX;
    }
    //taille max
    if((width<LARGEUR_MAX)&&(height<HAUTEUR_MAX)){
        if((float)width/LARGEUR_MAX>(float)height/HAUTEUR_MAX){
            r=(float)width/LARGEUR_MAX;
            width=LARGEUR_MAX;
            height=height/r;
        }
        else {
            r=(float)height/HAUTEUR_MAX;
            height=HAUTEUR_MAX;
            width=width/r;
        }
    }

    printf("image width : %d - height : %d \n", width, height);

    /* Creating the map */
    map = map_init(EPSG_4326, bornes.ymin, bornes.xmin, bornes.ymax, bornes.xmax, width, height);

    red = y_color(RED);
    yellow = y_color(YELLOW);

    map_point(map, argv[1], ROUND, 3, red, yellow);

    free(red);
    free(yellow);

    y_save_png(map->image, "point_map.png");
    map_destroy(map);

    return 0;
}
