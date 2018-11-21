/**
 * \file carte.c
 * Program showing geographic data.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fenetre.h"
#include "lecture_csv.h"
#include "lecture_grille.h"
#include "lecture_bln.h"
#include "outils.h"
#include "coordinates.h"
#include "affichage.h"

#include "yImage.h"

/* Names of datafiles */
#define DATA_DIR "/usr/local/share/geographie/donnees_France"
#define DATA_DIR_MONDE "/usr/local/share/geographie/donnees_monde"
#define RELIEF_GRD_FILE "grilles_relief/France_-_relief_1km.grd"
#define RELIEF_GRI_FILE "grilles_relief/France_1km.gr2"
#define RELIEF_GRD_FILE_HR "grilles_relief/France-250m.grd"
#define RELIEF_GRI_FILE_HR "grilles_relief/France_250m.gr2"
#define PREFECTURES_CVS_FILE "fichiers_postes/France_prefectures.csv"
#define RIVIERES_BLN_FILE "fonds_de_cartes/fonds_hydro/France_-_rivieres_principales.bln"
#define DEPARTEMENTS_BLN_FILE "fonds_de_cartes/France_et_departements.bln"
#define CENTRE_BLN_FILE "fonds_de_cartes/regions_economiques/Centre.bln"
#define PAYS_EUROPE "europe.bln"


/* global variables */

/* directories and files names */
char *data_dir;
char *data_file;

/* verbose mode */
int verbose;


void usage(char *nom){
    fprintf(stderr,"\nusage:\n%s [-options]\n\
\t -help : display this message\n\
\t -verbose\n\
\t -w <width> : indicate the window's width\n\
\t -h <height>\n\
\t -x1 xlamb_mini\n\
\t -x2 xlamb_maxi\n\
\t -y1 ylamb_mini\n\
\t -y2 ylamb_maxi\n\
\t -fin : high resolution \n\
\t -file fichier -datadir rep\n\
\t -hmax altitude_maxi : adapt the color range for relief\n\
", nom);

    exit(-1);
}


/* color ranges */
char world_color_scheme[] = "-999,0x000000,-990,0x00000000,-989,0x0000FF,0,0x0000FF,1,0x00FF00,615,0x00CCFF,1230,0x99FFFF,1845,0x33CC99,2460,0xCCFFCC,3075,0xE2FFDB,3690,0xFFFFDD,4305,0xFFE6B4,4920,0xCCB380,5535,0xFFFF66,6150,0x999933,6765,0x996633,7380,0xFF6600";
char europe_color_scheme[] =  "-999,0x000000,-990,0x000000,-989,0x0000FF,0,0x0000FF,1,0x00FF00,308,0x00CCFF,615,0x99FFFF,922,0x33CC99,1230,0xCCFFCC,1532,0xE2FFDB,1845,0xFFFFDD,2152,0xFFE6B4,2460,0xCCB380,2737,0xFFFF66,3075,0x999933,3282,0x996633,4650,0xFF6600";



/**
 * transform WGS84 data to Extended Lambert II data.
 */
static void geo2Lamb(bln_data_t *data) {

    bln_data_t *current = data;

    while(current != NULL) {
        int n;
        float xmin, xmax, ymin, ymax;
        for(n=0; n<current->nbPoints; n++) {
            coord_plane wgs;
            wgs.X = current->x[n];
            wgs.Y = current->y[n];
            coord_plane lamb = Wgs84_to_Lambert(wgs);
            current->x[n] = lamb.X;
            current->y[n] = lamb.Y;
            // find the new extremun
            if(n==0) {
                xmin=lamb.X;
                xmax=lamb.X;
                ymin=lamb.Y;
                ymax=lamb.Y;
            } else {
                if(lamb.X < xmin) {
                    xmin = lamb.X;
                } else if(lamb.X > xmax) {
                    xmax = lamb.X;
                }
                if(lamb.Y < ymin) {
                    ymin = lamb.Y;
                } else if(lamb.Y > ymax) {
                    ymax = lamb.Y;
                }
            }
        }
        current->xmin = xmin;
        current->xmax = xmax;
        current->ymin = ymin;
        current->ymax = ymax;
        current = current->next; 
    }
}


/**
 * Internal transformation to have data in hectometers.
 */
static void lambInHm(bln_data_t *data) {

    bln_data_t *current = data;

    while(current != NULL) {
        int n;
        for(n=0; n<current->nbPoints; n++) {
            current->x[n] = current->x[n]/100;
            current->y[n] = current->y[n]/100;
        }
        current->xmin = current->xmin/100;
        current->xmax = current->xmax/100;
        current->ymin = current->ymin/100;
        current->ymax = current->ymax/100;
        current = current->next; 
    }
}


static int draw_european_countries(fenetre fen) {
    char filename[1000];
    int err;    
    sprintf(filename, "%s/%s", DATA_DIR_MONDE, PAYS_EUROPE);

    bln_data_t *data = bln_read_file(filename, EPSG_4326);
    
    if(data == NULL) {
        return 1;
    }

    geo2Lamb(data);
    lambInHm(data);
    
    err = bln_show_in_window(data, fen, 1, BLANC, 1, MARRON);
    bln_destroy(data);
    return err;
}


int main(int argc, char **argv){
    int i; // counter
    int width, height, depth; // taille de la fenêtre d'affichage et nb de bits pour les couleurs
    float x1,y1,x2,y2; // max and min plane coordinates for the map
    char *file;
    FILE *fd; // data file
    grille_t *structure; // les valeurs lues dans l'entête
    /*float*//*int n;*/ //valeur lue dans le fichier
    //char lu[10]; // lecture d'une chaine
    int hmax, xmax, ymax; //altitude maximale et sa position en coordonnées pixel
    XEvent event; //evenement clavier ou souris
    //poste prefecture; // lecture du fichier postes
    fenetre fen; // la fenetre d'affichege de la carte
    int fin=0; // affichage haute résolution
    grille_t *reliefEurope; /* relief etopo sur une plus grande zone */
    grille_t *reliefZoneII; /* la même en lambert */

    /* init */
    verbose=0;
    width=0; height=0;
    x1=0; x2=0; y1=0; y2=0;
    hmax=0; xmax=0; ymax=0;


    /* Checking parameters */
    for (i=1; i<argc; i++) {
        if (!strcasecmp(argv[i], "-help")) usage(argv[0]);
        else
        if (!strcasecmp(argv[i], "-verbose")) verbose = 1;
        else
        if (i<argc-1 && !strncasecmp(argv[i], "-w", 2)) {
            width = atoi(argv[++i]);
            if (width<=10) width = 10;
        }
        else
        if (i<argc-1 && !strcasecmp(argv[i], "-h")) {
            height = atoi(argv[++i]);
            if (height<=5) height = 5;
        }
        else
        if (i<argc-1 && !strcasecmp(argv[i], "-x1")) {
            x1 = atoi(argv[++i]);
            //if (x1<=5) x1 = 5;
        }
        else
        if (i<argc-1 && !strcasecmp(argv[i], "-x2")) {
            x2 = atoi(argv[++i]);
            //if (x2<=5) x2 = 5;
        }
        else
        if (i<argc-1 && !strcasecmp(argv[i], "-y1")) {
            y1 = atoi(argv[++i]);
            //if (y1<=5) y1 = 5;
        }
        else
        if (i<argc-1 && !strcasecmp(argv[i], "-y2")) {
            y2 = atoi(argv[++i]);
            //if (y2<=5) y2 = 5;
        }
        else
        if (i<argc-1 && !strcasecmp(argv[i], "-hmax")) {
            hmax = atoi(argv[++i]);
            if (hmax<=200) hmax = 200;
        }
        else
        if (i<argc-1 && !strcasecmp(argv[i], "-file"))
            data_file = argv[++i];
        else
        if (i<argc-1 && !strcasecmp(argv[i], "-datadir"))
            data_dir = argv[++i];
        else
        if (!strcasecmp(argv[i], "-fin")) fin = 1;
        else
            usage(argv[0]);
    }
    /* End reading parameters */

    /* Checking */
    if((x1>x2)||(y1>y2)) usage(argv[0]);


    /* filename to use for relief */
    if (data_dir==NULL) data_dir = DATA_DIR;
    file = (char*)malloc((strlen(data_dir)+200)*sizeof(char));
    if (fin==0) sprintf(file, "%s/%s", data_dir, RELIEF_GRI_FILE);
    else sprintf(file, "%s/%s", data_dir, RELIEF_GRI_FILE_HR);

    if(data_file==NULL) {
        data_file=(char*)malloc((strlen(data_dir)+200)*sizeof(char));
        strcpy(data_file,file);
    }

    /* open file */
    if(!(fd=fopen(data_file, "r"))) {
        printf("Could not open file %s!\n", data_file);
        exit(-1);
    }

    if (verbose) {
        fprintf(stderr, "\nReading header data from '%s'\n", data_file);
    }

    structure=read_header_data(fd);

    //si la taille (height!=0) est choisie par parametre
    //pas de zoom
    if((x2!=0)||(x1!=0)){ // si une plage de x a été définie
        if ((transforme_x(x2,structure->width,structure->x1,structure->x2)-transforme_x(x1,structure->width,structure->x1,structure->x2)<width)||(width==0))  width=(transforme_x(x2,structure->width,structure->x1,structure->x2)-transforme_x(x1,structure->width,structure->x1,structure->x2));
    }
    if((y2!=0)||(y1!=0)){ // si une plage de y a été définie
        if ((transforme_y(y1,structure->height,structure->y1,structure->y2)-transforme_y(y2,structure->height,structure->y1,structure->y2)<height)||(height==0))  height=(transforme_y(y1,structure->height,structure->y1,structure->y2)-transforme_y(y2,structure->height,structure->y1,structure->y2));
    }

    if(width==0) width=structure->width+250*(!fin);
    if(height==0) height=structure->height-150*(!fin);
    if(x1==0) x1=structure->x1;
    if(x2==0) x2=structure->x2+2500*(!fin);
    if(y1==0) y1=structure->y1-1000*(!fin);
    if(y2==0) y2=structure->y2;

    fclose(fd);

    /* Open window */
    init_Xvariable();
    fen=cree_fenetre_coloree(width, height, &depth, x1, x2, y1, y2, BLEU);


    /* european countries */
    if(!fin){
        if(draw_european_countries(fen)!=0) {
            fprintf(stderr, "Error dealing with Europe datafile\n");
        }
    }

    /* Affichage de la grille de relief */
    if(verbose) fprintf(stderr, "\nReading data from '%s'\n", data_file);
    if(verbose) fprintf(stderr, "\nMap %dx%d\n\n",width, height);

    //affiche_grille(fen, "etopo2.gr_1", NULL);
    //affiche_grille(fen, "../etopo5.alt", NULL);
    //reliefEurope=lit_grille_entiere("../etopo5.alt");
    reliefEurope=lit_grille_entiere(/*"../etopo2.gr"*/"../../donnees_Etopo/etopo2v2c_i2_MSB.ALT");
    //reliefEurope=lit_grille_entiere("../../donnees_Etopo/etopo1_bed_c_i2.GR2");

    reliefEurope->type=EPSG_4326;
    info_grille(reliefEurope);
    reliefZoneII=transforme_vers_Lambert(reliefEurope);
    info_grille(reliefZoneII);
    if(reliefZoneII==NULL) fprintf(stdout, "Relief not available\n");
    else affiche_grille(fen, reliefZoneII, NULL, europe_color_scheme, depth, hmax);

    destroy_grille(reliefEurope);
    destroy_grille(reliefZoneII);

    /* departments */
    sprintf(file, "%s/%s", data_dir, DEPARTEMENTS_BLN_FILE);
    bln_data_t *departments = bln_read_file(file, EPSG_27572);
    bln_show_in_window(departments, fen, 2, ORANGE, 0, 0);
    bln_destroy(departments);

    /* rivers */
    sprintf(file, "%s/%s", data_dir, RIVIERES_BLN_FILE);
    bln_data_t *rivers = bln_read_file(file, EPSG_27572);
    bln_show_in_window(rivers, fen, 1+fin, BLEU, 0, 0);
    bln_destroy(rivers);

    /* points cities */
    sprintf(file, "%s/%s", data_dir, PREFECTURES_CVS_FILE);

    poste_t *points = read_points_file(file, EPSG_27572);
    if(points != NULL) {
        point_data(fen, points, ROND, 4+fin, MARRON_, NOIR);
        destroy_points_data(points);
    }


    /* ECHELLE  */
    trace_echelle(fen, ORANGE_, NOIR);

    /* SIGNATURE */
    //display_text(fen, 3, fen.h-35, "Programme d'affichage de données géographiques\nYannick Garcia - 2006", JAUNE);

    /* main loop */
    actualise_fenetre(fen.window);

    while(1) {
        XNextEvent(fen.window.dpy, &event);
        i = manage_event(fen.window, event);
        if (i==2) break;
    }

    if(verbose) fprintf(stderr, "\nMaximal altitud : %d, for X=%d, Y=%d\n\n", hmax, xmax, ymax);

    /* free memory and close display */
    free(file);
    fermeture(fen);
    printf("End of program\n");

    return 0;
}
