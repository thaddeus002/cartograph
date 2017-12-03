/** \file pointe.c */

#include "map.h"
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

    bln_boundaries_t bornes; // boundaries of data file
    bln_data_t *data;
    int width, height; // taille de la fenetre
    float temp;//aide au calcul
    map_t *map; // where make the drawing
    float r; //ratio pour l'agrandissement
    char leg[20]; // legende des meridiens
    yColor *red, *yellow;


    if(argc<2) usage(argv[0]);
    printf("File to points : %s\n", argv[1]);

    data=bln_read_file(argv[1]);
    bln_find_data_boundaries(data, &bornes);
    bln_destroy(data);
    if(bornes.result!=0) {
        fprintf(stderr,"Bad file format : %s\n", argv[1]);
        exit(bornes.result);
    }

    printf("found boundaries : %f,%f - %f,%f\n", bornes.xmin, bornes.xmax, bornes.ymin, bornes.ymax);
    width=(bornes.xmax-bornes.xmin)*10;
    height=(bornes.ymax-bornes.ymin)*10;
    temp=bornes.xmax-bornes.xmin;

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

    printf("largeur : %d - hauteur : %d \n", width, height);


    /* Creating the map */
    map = map_init(EPSG_4326, bornes.ymin, bornes.xmin, bornes.ymax, bornes.xmax, width, height);


    /* marquage de l'équateur, des tropiques, et cercles polaires */
/*  trace_ligne(f,bornes.xmin,bornes.xmax,0,0,JAUNE,0);
    display_text(f, transforme_x(bornes.xmin,width,bornes.xmin,bornes.xmax)+10,transforme_y(0,height,bornes.ymin,bornes.ymax)-1,"Equateur", JAUNE);
    trace_ligne(f,bornes.xmin,bornes.xmax,23.45,23.45,JAUNE,0);
    display_text(f, transforme_x(bornes.xmin,width,bornes.xmin,bornes.xmax)+10,transforme_y(23.45,height,bornes.ymin,bornes.ymax)-1,"Tropique du Cancer", JAUNE);
    trace_ligne(f,bornes.xmin,bornes.xmax,-23.45,-23.45,JAUNE,0);
    display_text(f, transforme_x(bornes.xmin,width,bornes.xmin,bornes.xmax)+10,transforme_y(-23.45,height,bornes.ymin,bornes.ymax)-1,"Tropique du Capricorne", JAUNE);
    trace_ligne(f,bornes.xmin,bornes.xmax,66.55,66.55,JAUNE,0);
    display_text(f, transforme_x(bornes.xmin,width,bornes.xmin,bornes.xmax)+10,transforme_y(66.55,height,bornes.ymin,bornes.ymax)-1,"Cercle polaire", JAUNE);
    trace_ligne(f,bornes.xmin,bornes.xmax,-66.55,-66.55,JAUNE,0);
    display_text(f, transforme_x(bornes.xmin,width,bornes.xmin,bornes.xmax)+10,transforme_y(-66.55,height,bornes.ymin,bornes.ymax)-1,"Cercle polaire", JAUNE);
*/
    /* marquage de quelques meridiens */
/*  for(i=-120;i<=180;i=i+60){
        trace_ligne(f,i,i,bornes.ymin,bornes.ymax,JAUNE,0);
        sprintf(leg,"%d°",i);
        display_text(f, transforme_x(i,width,bornes.xmin,bornes.xmax)+4,transforme_y(bornes.ymax,height,bornes.ymin,bornes.ymax)+15,leg, JAUNE);
    }
*/

    red = y_color(RED);
    yellow = y_color(YELLOW);

    map_point(map, argv[1], ROUND, 3, red, yellow);

    free(red);
    free(yellow);

    y_save_png(map->image, "point_map.png");
    map_destroy(map);


    return 0;
}
