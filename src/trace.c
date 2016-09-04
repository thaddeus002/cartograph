/**
 * \file trace.c
 * Draw a map using the content of a bln file.
 */


#include "lecture_csv.h"
#include "outils.h"
#include "map.h"
#include "ySaveImage.h"
#include <stdio.h>
#include <string.h>


#define LARGEUR_MAX 1400
#define HAUTEUR_MAX 800

/**
 * The usage function
 */
static void usage(char *prog){
    fprintf(stderr,"Draws the lines listed in a BLN file.\n");
    fprintf(stderr,"Usage : %s <file.bln> [<pointage.csv>]\n", prog);
    exit(1);
}

/**
 * Main program
 */
int main(int argc, char **argv){

    bln_boundaries_t *bornes;
    int width, height; // image size
    float r; //zoom ratio



    map_t *map;
    yColor *backColor, *countriesColor;



    if(argc<2) usage(argv[0]);
    printf("Fichier à afficher : %s\n", argv[1]);

    bornes=bln_find_boundaries(argv[1]);
    if(bornes->result!=0) {
        fprintf(stderr,"Pb de format du fichier %s\n", argv[1]);
        exit(bornes->result);
    }

    printf("bornes trouvées : %f,%f - %f,%f : %d\n", bornes->xmin, bornes->xmax, bornes->ymin, bornes->ymax, bornes->result);
    width=(bornes->xmax-bornes->xmin)*10;
    height=(bornes->ymax-bornes->ymin)*10;
    free(bornes);

    if((width<0)|| (height<0)) {
        printf("Erreur dans les bornes... Arrêt du programme\n");
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


    /* Création de l'image */

    backColor = y_color(BLUE);
    countriesColor= y_color(WHITE);
    map = map_init(EPSG_4326, bornes->ymin, bornes->xmin, bornes->ymax, bornes->xmax, width, height);
    map_set_background(map, backColor);
    map_trace_bln(map, argv[1], countriesColor);
    free(backColor);
    free(countriesColor);





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

    // POINTAGE
    //if(argc==3) traite_csv(f,argv[2],ROND,3,ROUGE,JAUNE);


    // END
    sauve_png(map->image, "output_trace_y.png");
    map_destroy(map);

    return 0;
}
