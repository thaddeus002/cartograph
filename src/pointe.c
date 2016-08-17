/** \file pointe.c */

#include "lecture_bln.h"
#include "lecture_csv.h"
#include "outils.h"
#include <stdio.h>
#include <string.h>

#define LARGEUR_MAX 1400
#define HAUTEUR_MAX 800

/* La fonction usage */
void usage(char *prog){
    fprintf(stderr,"Pointage des lieux d'un fichier au format CSV.\n");
    fprintf(stderr,"Usage : %s <pointage.csv>\n", prog);
    exit(1);
}

/* Programme principal */
int main(int argc, char **argv){

    bornes_bln bornes; // bornes du fichier à lire
    int width, height; // taille de la fenetre
    float temp;//aide au calcul
    fenetre f; // fenetre d'affichage du tracé
    int depth; // nb de couleurs?
    XEvent event; //evenement clavier ou souris
    int i; //numero d'evenement clavier
    float r; //ratio pour l'agrandissement
    char leg[20]; // legende des meridiens


    if(argc<2) usage(argv[0]);
    printf("Fichier à afficher : %s\n", argv[1]);

    bornes=cherche_bornes_bln(argv[1]);
    if(bornes.resultat!=0) {
        fprintf(stderr,"Pb de format du fichier %s\n", argv[1]);
        exit(bornes.resultat);
    }

    printf("bornes trouvées : %f,%f - %f,%f : %d\n", bornes.xmin, bornes.xmax, bornes.ymin, bornes.ymax, bornes.resultat);
    width=(bornes.xmax-bornes.xmin)*10;
    height=(bornes.ymax-bornes.ymin)*10;
    temp=bornes.xmax-bornes.xmin;

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


    /* Création de la fenêtre */
    init_Xvariable();
    f=cree_fenetre_coloree(width, height, &depth, bornes.xmin, bornes.xmax, bornes.ymin, bornes.ymax, BLEU);


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

    traite_csv(f,argv[1],ROND,3,ROUGE,JAUNE);

    /* Boucle principale*/
    while(1) {
        XNextEvent(f.dpy, &event);
        i = manage_event(f, event);
        if (i==2) break;
    }

    /* fermeture des display */
    fermeture(f);
    return 0;
}
