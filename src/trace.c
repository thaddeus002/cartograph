/**
 * \file trace.c
 * Draw a map using the content of a bln file.
 */


#include "lecture_csv.h"
#include "outils.h"
#include "map.h"
#include "yImage_io.h"
#include "yder.h"
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
 * As we don't know what is the data domain, we better choose image's
 * size at execution time.
 * \param width must be allocated
 * \param heigth must be allocated
 */
static int choose_image_size(char *blnfile, int *width, int *height) {

    bln_boundaries_t *bornes;
    float r; //zoom ratio

    bornes=bln_find_boundaries(blnfile);
    if(bornes->result!=0) {
        y_log_message(Y_LOG_LEVEL_ERROR, "Error in format of file %s", blnfile);
        return(bornes->result);
    }

    y_log_message(Y_LOG_LEVEL_DEBUG, "Found BLN boundaries : %f,%f - %f,%f : %d", bornes->xmin, bornes->xmax, bornes->ymin, bornes->ymax, bornes->result);
    *width=(bornes->xmax-bornes->xmin)*10;
    *height=(bornes->ymax-bornes->ymin)*10;
    free(bornes);

    if((*width<0)|| (*height<0)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "Error in boundaries... Stopping program");
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

    y_log_message(Y_LOG_LEVEL_DEBUG, "width : %d - height : %d", *width, *height);
    return 0;
}



/**
 * Main program
 */
int main(int argc, char **argv){

    int width, height; // image size
    int err;
    map_t *map;
    yColor *backColor, *countriesColor;

    y_init_logs(argv[0], Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, NULL);

    if(argc<2) usage(argv[0]);
    y_log_message(Y_LOG_LEVEL_DEBUG, "File to draw : %s", argv[1]);

    err = choose_image_size(argv[1], &width, &height);
    if(err) {
        y_log_message(Y_LOG_LEVEL_ERROR, "Exiting program");
        return err;
    }

    /* Create image */

    backColor = y_color(BLUE);
    countriesColor= y_color(WHITE);
    map = map_create_with_bln(argv[1], backColor, countriesColor, EPSG_4326, width, height);
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
    y_log_message(Y_LOG_LEVEL_INFO, "Creating file %s", "output_trace_y.png");
    y_save_png(map->image, "output_trace_y.png");
    map_destroy(map);

    y_close_logs();
    return 0;
}
