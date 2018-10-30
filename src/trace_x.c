/**
 * \file trace_x.c
 * Draw a map using the content of a bln file.
 * This is the same goal than trace.c but using the xlib.
 */


#include "lecture_bln.h"
#include "bln.h"
#include "lecture_csv.h"
#include "outils.h"
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

    bln_boundaries_t bornes; // bornes du fichier à lire
    int width, height; // taille de la fenetre
    float temp;//aide au calcul
    fenetre f; // fenetre d'affichage du tracé
    int depth; // nb de couleurs?
    XEvent event; //evenement clavier ou souris
    int i; //numero d'evenement clavier
    float r; //ratio pour l'agrandissement
    int err; // error code


    if(argc<2) usage(argv[0]);
    printf("File to show : %s\n", argv[1]);

    bln_data_t *data = bln_read_file(argv[1]);
    bln_find_data_boundaries(data, &bornes);

    if(bornes.result!=0) {
        fprintf(stderr,"Incorrect file format : %s\n", argv[1]);
        bln_destroy(data);
        exit(bornes.result);
    }

    printf("boundaries found : %f,%f - %f,%f : %d\n", bornes.xmin, bornes.xmax, bornes.ymin, bornes.ymax, bornes.result);
    width=(bornes.xmax-bornes.xmin)*10;
    height=(bornes.ymax-bornes.ymin)*10;
    temp=bornes.xmax-bornes.xmin;

    if((width<0)|| (height<0)) {
        printf("Boundaries error... Stopping program\n");
        bln_destroy(data);
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

    printf("width : %d - height : %d \n", width, height);

    /* create the window */
    init_Xvariable();
    f=cree_fenetre_coloree(width, height, &depth, bornes.xmin, bornes.xmax, bornes.ymin, bornes.ymax, BLEU);

    trace_bln_lignes(argv[1], f, 0, BLANC, 1, VERT);
    bln_destroy(data);

    if(argc==3) traite_csv(f,argv[2],ROND,3,ROUGE,JAUNE);

    /* main loop */
    while(1) {
        XNextEvent(f.dpy, &event);
        i = manage_event(f, event);
        if (i==2) break;
    }

    /* close display */
    sauve_fenetre_png(&f, "output_trace.png");
    fermeture(f);
    return 0;
}
