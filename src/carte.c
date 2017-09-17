/**
 * \file carte.c
 * Program showing geographic data. */
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
#include "coordonnees.h"
#include "affichage.h"

#include "yImage.h"

/* Names of datafiles */
#define DATA_DIR "/usr/local/share/geographie/donnees_France"
#define DATA_DIR_MONDE "/usr/local/share/geographie/donnees_monde"
#define RELIEF_GRD_FILE "grilles relief/France - relief 1km.grd"
#define RELIEF_GRI_FILE "grilles relief/France_1km.gr2"
#define RELIEF_GRD_FILE_HR "grilles relief/France-250m.grd"
#define RELIEF_GRI_FILE_HR "grilles relief/France_250m.gr2"
#define PREFECTURES_CVS_FILE "fichiers postes/France - préfectures.csv"
#define RIVIERES_BLN_FILE "fonds de cartes/fonds hydro/France - rivières principales.bln"
#define DEPARTEMENTS_BLN_FILE "fonds de cartes/France_et_departements.bln"
#define CENTRE_BLN_FILE "fonds de cartes/régions économiques/Centre.bln"
#define PAYS_EUROPE "europe.bln"


/* global variables */

/* répertoires et noms de fichiers */
char *data_dir;
char *data_file;

/* verbose mode */
int verbose;


void usage(char *nom){
    fprintf(stderr,"\nusage:\n%s [-options]\n\
\t -help : display this message\n\
\t -verbose\n\
\t -w largeur : impose la largeur de la fenêtre\n\
\t -h hauteur\n\
\t -x1 xlamb_mini\n\
\t -x2 xlamb_maxi\n\
\t -y1 ylamb_mini\n\
\t -y2 ylamb_maxi\n\
\t -fin : haute résolution \n\
\t -file fichier -datadir rep\n\
\t -hmax altitude_maxi : adaptation plage de couleurs pour le relief\n\
", nom);

    exit(-1);
}


/* les plages de couleur */
//char color_scheme[] = "-999,0x000000,-990,0x00000000,-989,0x0000FF,0,0x0000FF,1,0x00FF00,615,0x00CCFF,1230,0x99FFFF,1845,0x33CC99,2460,0xCCFFCC,3075,0xE2FFDB,3690,0xFFFFDD,4305,0xFFE6B4,4920,0xCCB380,5535,0xFFFF66,6150,0x999933,6765,0x996633,7380,0xFF6600";
char color_scheme[] =  "-999,0x000000,-990,0x000000,-989,0x0000FF,0,0x0000FF,1,0x00FF00,308,0x00CCFF,615,0x99FFFF,922,0x33CC99,1230,0xCCFFCC,1532,0xE2FFDB,1845,0xFFFFDD,2152,0xFFE6B4,2460,0xCCB380,2737,0xFFFF66,3075,0x999933,3282,0x996633,4650,0xFF6600";



int main(int argc, char **argv){
    int i/*,j*/; //compteurs
    int width, height, depth; // taille de la fenêtre d'affichage et nb de bits pour les couleurs
    float x1,y1,x2,y2; //coordonnées lambert max et min
    char *file;
    FILE *fd; //fichier de donnees
    //FILE *fd_cvs; // fichier de postes
    //int header_data; // longueur de l'entête ?
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

    /*init*/
    verbose=0;
    width=0; height=0;
    x1=0; x2=0; y1=0; y2=0;
    hmax=0; xmax=0; ymax=0;


    /* Vérification des paramètres */
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
    /* Fin de lecture des paramètres */

    /* Vérifications */
    if((x1>x2)||(y1>y2)) usage(argv[0]);


    /* détermination du nom du fichier à utiliser */
    if (data_dir==NULL) data_dir = DATA_DIR;
    file = (char*)malloc((strlen(data_dir)+200)*sizeof(char));
    if (fin==0) sprintf(file, "%s/%s", data_dir, RELIEF_GRI_FILE);
    else sprintf(file, "%s/%s", data_dir, RELIEF_GRI_FILE_HR);

    if(data_file==NULL) {
        data_file=(char*)malloc((strlen(data_dir)+200)*sizeof(char));
        strcpy(data_file,file);
    }

    /* ouverture du fichier */
    if(!(fd=fopen(data_file, "r"))) {
        printf("Ouverture de %s impossible!\n", data_file);
        exit(-1);
    }
    else if(verbose) fprintf(stderr, "Fichier %s ouvert!\n", data_file);

    if (verbose)
        fprintf(stderr, "\nReading header data from '%s'\n", data_file);


    structure=read_header_data(fd);

    //si la taille e(height!=0)st choisie par parametre
    //pas de zoom
    if((x2!=0)||(x1!=0)){ // si une plage de x a été définie
        if ((transforme_x(x2,structure->width,structure->x1,structure->x2)-transforme_x(x1,structure->width,structure->x1,structure->x2)<width)||(width==0))  width=(transforme_x(x2,structure->width,structure->x1,structure->x2)-transforme_x(x1,structure->width,structure->x1,structure->x2));
    }
    if((y2!=0)||(y1!=0)){ // si une plage de y a été définie
        if ((transforme_y(y1,structure->height,structure->y1,structure->y2)-transforme_y(y2,structure->height,structure->y1,structure->y2)<height)||(height==0))  height=(transforme_y(y1,structure->height,structure->y1,structure->y2)-transforme_y(y2,structure->height,structure->y1,structure->y2));
    }

    /*if( (x2!=0)&&(x1!=0)&&((x2-x1<width)|| (width==0)) ) width=(x2-x1)/2;
    if( (y2!=0)&&(y1!=0)&&((y2-y1<height)|| (height==0)) ) height=(y2-y1)/2; // pourquoi /2 ?*/


    if(width==0) width=structure->width+250*(!fin);//+400;
    if (height==0) height=structure->height-150*(!fin);
    if(x1==0) x1=structure->x1/*-1500*/;
    if(x2==0) x2=structure->x2+2500*(!fin);
    if(y1==0) y1=structure->y1-1000*(!fin);
    if(y2==0) y2=structure->y2;//*0.93;
//  width=(x2-x1)/10+1; height=(y2-y1)/10+1;


    fclose(fd);

    /* Ouverture fenêtre */
    init_Xvariable();
    //fen=cree_fenetre(width, height, &depth, x1, x2, y1, y2);
    fen=cree_fenetre_coloree(width, height, &depth, x1, x2, y1, y2, BLEU);

    init_colors(color_scheme,depth,1, hmax);



    /* european countries */
    if(!fin){
        sprintf(file, "%s/%s", DATA_DIR_MONDE, PAYS_EUROPE);
        if(trace_bln_geo(file, fen, 1, BLANC, 1, MARRON)!=0) fprintf(stderr, "Problème avec le fichier Europe\n");
    }

    /* Affichage de la grille de relief */
    if(verbose) fprintf(stderr, "\nReading data from '%s'\n", data_file);
    if(verbose) fprintf(stderr, "\nCarte %dx%d\n\n",width, height);

    //affiche_grille(fen, "etopo2.gr_1", NULL);
    //affiche_grille(fen, "../etopo5.alt", NULL);
    //reliefEurope=lit_grille_entiere("../etopo5.alt");
    reliefEurope=lit_grille_entiere(/*"../etopo2.gr"*/"../../donnees_Etopo/etopo2v2c_i2_MSB.ALT");
    //reliefEurope=lit_grille_entiere("../../donnees_Etopo/etopo1_bed_c_i2.GR2");
    reliefEurope->type=LATLON;
    info_grille(reliefEurope);
    //reliefZoneII=NULL;
    reliefZoneII=transforme_vers_Lambert(reliefEurope);
    info_grille(reliefZoneII);
    if(reliefZoneII==NULL) fprintf(stdout, "Relief de zone non disponible\n");
    else affiche_grille_(fen, reliefZoneII, NULL);
    destroy_grille(reliefEurope);
    destroy_grille(reliefZoneII);
    //affiche_grille(fen, data_file, NULL);


    /* départements */
    sprintf(file, "%s/%s", data_dir, DEPARTEMENTS_BLN_FILE);
    if(trace_bln_lignes(file, fen, 2, ORANGE, 0, 0)!=0) fprintf(stderr, "Problème avec le fichier départements\n");

    /* centre marqué plus fort */
    /*sprintf(file, "%s/%s", data_dir, CENTRE_BLN_FILE);
    if(trace_bln_points(file, fen, 2, ORANGE, 0, 0)!=0) fprintf(stderr, "Problème avec le fichier centre\n");*/

    /* les rivieres */
    //sprintf(file, "%s/%s", data_dir, RIVIERES_BLN_FILE);
    //if(trace_bln_lignes(file, fen, 1+fin, BLEU, 0, 0)!=0) fprintf(stderr, "problème avec le fichier rivières\n");

    /* pointages */
    sprintf(file, "%s/%s", data_dir, PREFECTURES_CVS_FILE);
    traite_csv(fen, file, ROND, 4+fin, MARRON, NOIR);


    /* ECHELLE  */
    trace_echelle(fen, ORANGE, NOIR);

    /* SIGNATURE */
    //display_text(fen, 3, fen.h-35, "Programme d'affichage de données géographiques\nYannick Garcia - 2006", JAUNE);

    /*boucle principale*/
    /*XSetWindowBackgroundPixmap(fen.dpy, fen.win, pixmap);
    XClearWindow(fen.dpy, fen.win); */
    actualise_fenetre(fen);

    //i=1;
    while(1) {
        XNextEvent(fen.dpy, &event);
        i = manage_event(fen, event);
        if (i==2) break;
    }

    //if(verbose) fprintf(stderr, "\nAltitude maximale : %d, pour X=%d, Y=%d\n\n", hmax, xmax, ymax);

    /* libération de la mémoire et fermeture des display */
    free(file);
    fermeture(fen);
    printf("Fin du programme\n");

    return 0;
}
