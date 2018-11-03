/**
 * \file trace_x.c
 * \brief Draw a map using the content of a bln file.
 * This is the same goal than trace.c but using the xlib.
 */


#include "lecture_bln.h"
#include "bln.h"
#include "lecture_csv.h"
#include "outils.h"
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
 * \param width must be allocated
 * \param heigth must be allocated
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
    fenetre f; // drawing window
    int depth; // nb de couleurs?
    XEvent event; // evenement clavier ou souris
    int i; // numero d'evenement clavier


    y_init_logs(argv[0], Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, NULL);

    if(argc<2) usage(argv[0]);
    y_log_message(Y_LOG_LEVEL_DEBUG, "File to draw : %s", argv[1]);

    bln_data_t *data = bln_read_file(argv[1]);

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
        return err;
    }


    /* create the window */
    init_Xvariable();
    f=cree_fenetre_coloree(width, height, &depth, bornes.xmin, bornes.xmax, bornes.ymin, bornes.ymax, BLEU);

    bln_show_in_window(data, f, 0, BLANC, 1, VERT);
    bln_destroy(data);

    if(argc==3) traite_csv(f,argv[2],ROND,3,ROUGE,JAUNE);

    /* main loop */
    while(1) {
        XNextEvent(f.dpy, &event);
        i = manage_event(f, event);
        if (i==2) break;
    }

    /* close display */
    y_log_message(Y_LOG_LEVEL_INFO, "Creating file %s", "output_trace_y.png");
    sauve_fenetre_png(&f, "output_trace.png");
    fermeture(f);
    y_close_logs();
    return 0;
}

