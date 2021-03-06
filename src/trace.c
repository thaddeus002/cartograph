/**
 * \file trace.c
 * \brief Draw a map using the content of a bln file.
 */


#include "map.h"
#include "yImage_io.h"
#include "yder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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
 * \param bornes data's boundaries
 * \param width must be allocated
 * \param heigth must be allocated
 * \return 0 in case of success
 */
static int choose_image_size(bln_boundaries_t *bornes, int *width, int *height) {

    float r; //zoom ratio

    *width=(bornes->xmax-bornes->xmin)*10;
    *height=(bornes->ymax-bornes->ymin)*10;

    if((*width<0)|| (*height<0)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "Boundaries error");
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
    bln_boundaries_t bornes; // data's boundaries
    int err; // error code
    map_t *map;
    yColor *backColor, *countriesColor, *fillColor;
    yColor *meridian_c; // color for the meridians and parallels
    yColor *points_c, *text_c;

    y_init_logs(argv[0], Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, NULL);

    if(argc<2) usage(argv[0]);
    y_log_message(Y_LOG_LEVEL_DEBUG, "File to draw : %s", argv[1]);

    bln_data_t *data = bln_read_file(argv[1], EPSG_4326);

    bln_find_data_boundaries(data, &bornes);

    if(bornes.result!=0) {
        y_log_message(Y_LOG_LEVEL_ERROR, "Incorrect file format : %s", argv[1]);
        bln_destroy(data);
        exit(bornes.result);
    }

    y_log_message(Y_LOG_LEVEL_DEBUG, "boundaries found : %f,%f - %f,%f", bornes.xmin, bornes.xmax, bornes.ymin, bornes.ymax);


    err = choose_image_size(&bornes, &width, &height);
    if(err) {
        y_log_message(Y_LOG_LEVEL_ERROR, "Exiting program");
        bln_destroy(data);
        return err;
    }

    /* Create image */

    backColor = y_color(BLUE);
    countriesColor= y_color(MARRON);
    fillColor=y_color(GREEN);
    map = map_init(EPSG_4326, bornes.ymin, bornes.xmin, bornes.ymax, bornes.xmax, width, height);
    map_set_background(map, backColor);
    map_trace_bln_data(map, data, countriesColor, fillColor);
    bln_destroy(data);
    free(backColor);
    free(countriesColor);
    free(fillColor);

    meridian_c = y_color(YELLOW);
    map_draw_meridians(map, meridian_c);
    free(meridian_c);


    // POINTAGE
    if(argc==3) {
        points_c = y_color(RED);
        text_c = y_color(WHITE);
        poste_t *points = read_points_file(argv[2], EPSG_4326);
        map_point(map, points, ROUND, 3, points_c, text_c);
        // free memory
        destroy_points_data(points);
        free(points_c);
        free(text_c);
    }

    // END
    y_log_message(Y_LOG_LEVEL_INFO, "Creating file %s", "output_trace_y.png");
    y_save_png(map->image, "output_trace_y.png");
    map_destroy(map);

    y_close_logs();
    return 0;
}

