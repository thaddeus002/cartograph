/** \file lecture_bln.c */
/* fonctions de tracé de données de contours présentes dans un fichier au
** format BLN */


#include "lecture_bln.h"
#include "outils.h"
#include <stdio.h>
#include <stdlib.h>


#define LIGNE 250 /* longueur maximale des lignes du fichier, au dela on n'en tient pas compte */


/*fct interne */
/* ajout de geo le 12/04/2007 pour les fichiers en lon et lat */
/****************************************************/
/*               LA FONCTION DE TRACE               */
/****************************************************/
/* Interprète le contenu du fichier dans la fenetre de visualisation */
/* renvoie une valeur differente de 0 en cas d'échec */
int trace_bln_V2(char *fichier, fenetre f, int largeur, couleur c, int ligne, int geo, int remplir, couleur remplissage){
/* fichier : nom du fichier BLN
** f : fenetre où doit se faire le tracé
** largeur : largeur des lignes
** c : couleur des lignes
** ligne : 1 si les points sont rejoints par une ligne */
/* ajout de l'argument "geo" le 12/04/2007 pour les fichiers en lon et lat */
/* remplir : 1 s'il faut remplir les formes fermées
** remplissage : couleur à l'intérieur des formes */



    /* variables */
    FILE *fd; /* Descripteur pour l'ouverture du fichier */
    char buf_read[LIGNE]; /* lecture d'une ligne */
    int N; /* nb de valeurs (de lignes) pour un trait */
    int i; /* numéro de ligne de coordonnées */
    int j; /* compteur de boucle */

    char X[20], Y[20]; //lecture d'entier ou de reel selon les cas
    float a, b; // coordonnées d'un point
    int x, y; // transformés en coordonnées fenetres

    int k; char pays[100], region[100]; // infos facultatives
    pays[0]='\0';

    float Xm,Ym; //calcul de la position moyenne du tracé
    float Xmax,Xmin,Ymax,Ymin; // même but mais autre méthode

    XPoint *points; /* liste des points à relier */
    XPoint coordonnees; /* lu sur une ligne */


    /* ouverture du fichier en lecture */
    fd=fopen(fichier, "r");
    if(fd==NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", fichier);
        return(1);
    }

    /* ajustement */
    if(largeur>10) largeur=10;

    /* initialisations */
    a=0; b=0;
    i=0;


    /* Lecture ligne par ligne */
    while(fgets(buf_read, LIGNE, fd)!=NULL){

        if(i==0) { /* ligne d'entete */
            sscanf(buf_read, "%d,%d,%[a-zA-Z'\" -],%s", &N, &k, pays, region);
            if (N==0) continue; /* i reste à 0 pour la lecture de la prochaine ligne */
        }

        else { /* ligne de donnée : coordonnée d'un point */

            if(i==1){
                points=malloc(N*sizeof(XPoint));
                if(points==NULL){
                    fprintf(stderr, "Allocation mémoire impossible\n");
                    return(3);
                }
            }


            /* lecture d'un enregistrement */
            if(sscanf(buf_read, "%[0-9.-],%[0-9.-]", X, Y)==0) return(2);
            a=atof(X);
            b=atof(Y);


            /* ajout du 12/04/2007 pour les fichiers en lon et lat */
            if(geo){
                coord_geo geo;
                coord_lamb lamb;

                geo.lambda=a; geo.phi=b;
                lamb=correction_WGS84(calcule_Lambert(geo));

                a=lamb.X/100; b=lamb.Y/100; //cartographie en hectometres
            }


            if(i==1){

                //Xm=cc;//transforme_xLambert(cc, f.w, f.x1, f.x2);
                //Ym=d;//transforme_yLambert(d, f.h, f.y1, f.y2);
                /* sur un tracé fermé, le premier point est répété une fois */
                Xmin=a; Xmax=a;
                Ymin=b; Ymax=b;
            }

            if(ligne==0)
                pointe(f,a,b,largeur,c,CARRE);
            else {

                //x=transforme_xLambert(a, f.w, f.x1, f.x2);
                //y=transforme_yLambert(b, f.h, f.y1, f.y2);
                x=transforme_x(a, f.w, f.x1, f.x2);
                y=transforme_y(b, f.h, f.y1, f.y2);


                coordonnees.x=x; coordonnees.y=y;
                points[i-1]=coordonnees;

                Xm+=a; Ym+=b;
                if(a<Xmin) Xmin=a; else if(a>Xmax) Xmax=a;
                if(b<Ymin) Ymin=b; else if(b>Ymax) Ymax=b;
            }

        }


        /* evolution des compteurs */
        if(i==N) {
            if(ligne!=0) {
                if((remplir)&&(points[0].x==points[N-1].x)&&(points[0].y==points[N-1].y)) {
                    remplit(f, points, N, remplissage);
                }
                trace_lignes(f, points, N, c, largeur);
            }
            free(points);

            Xm=Xm/N; Ym=Ym/N;
            /*suppression des guillemets*/
            if(pays[0]=='"') {
                if (pays[1]=='"') pays[0]='\0';
                else {
                    j=0;
                    while((pays[j+1]!='"')&&(j+1<strlen(pays))) {
                        pays[j]=pays[j+1];
                        j++;
                    }
                    pays[j]='\0';
                }
            }

            //if((pays[0]!='\0')&&(N>=20) &&(Xm>f.x1) &&(Xm<f.x2) &&(Ym>f.y1) &&(Ym<f.y2)  ) display_text(f,transforme_x(Xm, f.w, f.x1, f.x2)-(strlen(pays))*4,transforme_y(Ym, f.h, f.y1, f.y2),pays,ROUGE);
            if((pays[0]!='\0')&&(N>=2)) display_text(f,transforme_x(Xm, f.w, f.x1, f.x2)-(strlen(pays))*4,transforme_y(Ym, f.h, f.y1, f.y2),pays,ROUGE);
            /*autre positionnement du texte*/
            Xm=0.5*(Xmin+Xmax); Ym=0.5*(Ymin+Ymax);
            //if((pays[0]!='\0')&&(N>=20) &&(Xm>f.x1) &&(Xm<f.x2) &&(Ym>f.y1) &&(Ym<f.y2)  ) display_text(f,transforme_xLambert(Xm, f.w, f.x1, f.x2)-(strlen(pays))*4,transforme_yLambert(Ym, f.h, f.y1, f.y2),pays,VERT);
            if((pays[0]!='\0')&&(N>=2) ) display_text(f,transforme_x(Xm, f.w, f.x1, f.x2)-(strlen(pays))*4,transforme_y(Ym, f.h, f.y1, f.y2),pays,VERT);


            i=0;
            pays[0]='\0';
        }
        else i++;

    } /* Arrivée à la fin du fichier */

    fclose(fd);
    //if (points!=NULL) free(points);
    return(0);
}
/*******************************/
/* Fin de la fct trace_bln_V2  */
/*******************************/













/* passer un nom de fichier bln en paramètre et on s'occupe de tout */
/* retourne un code d'erreur non nul en cas d'échec */
int trace_bln_points(char *fichier, fenetre f, int largeur, couleur c, int remplir, couleur remplissage){

    return(trace_bln_V2(fichier,f, largeur, c,0, 0, remplir, remplissage));
}

/* passer un nom de fichier bln en paramètre et on s'occupe de tout */
/* retourne un code d'erreur non nul en cas d'échec */
int trace_bln_lignes(char *fichier, fenetre f, int largeur, couleur c, int remplir, couleur remplissage){

    return(trace_bln_V2(fichier, f, largeur, c,1 , 0, remplir, remplissage));
}


/* passer un nom de fichier bln en paramètre et on s'occupe de tout */
/* retourne un code d'erreur non nul en cas d'échec */
/* le fichier d'entrée contient des coordonnées en lon et lat */
int trace_bln_geo(char *fichier, fenetre f, int largeur, couleur c, int remplir, couleur remplissage){
    return(trace_bln_V2(fichier, f, largeur, c,1 , 1, remplir, remplissage));
}



bornes_bln cherche_bornes_bln(char *file){
    FILE *fd;
    char buf_read[250]; //lecture d'une ligne
    int N; //nb de valeurs sur le contour en cours
    int i;  //compteur
    // i==0 sur une ligne descriptive, sinon i=numéro de ligne
    float a, b; //, cc, d; // coordonnées de 2 points consécutifs
    //int x, y, z, t; // transformés en coordonnées fenetres
    char X[20], Y[20]; //lecture d'entier ou de reel selon les cas
    bornes_bln bornes; // à retourner
    float xmin=0; float xmax=0; float ymin=0; float ymax=0;
    bornes.resultat=1;
    int deb=1; //indique qu'on est au début du fichier

    fd=fopen(file, "r");
    if(fd==NULL) {
        fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", file);
        return(bornes);
    }

    i=0;

    while(fgets(buf_read, 250, fd)!=NULL){
        if(i==0) {
            sscanf(buf_read, "%d", &N);
            if (N==0) continue;
        }
        else {
        //a=cc; b=d;
            if(sscanf(buf_read, "%[0-9.-],%[0-9.-]", X, Y)==0) //return(bornes);
            break;

            a=atof(X);
            b=atof(Y);
            if(deb==1){
                xmin=a; xmax=a;
                ymin=b; ymax=b;
                deb=0;
            } else {
                if(a<xmin) xmin=a; else if(a>xmax) xmax=a;
                if(b<ymin) ymin=b; else if(b>ymax) ymax=b;
            }

        }
        if(i==N) i=0;
        else i++;
    }

    fclose(fd);

    bornes.xmin=xmin; bornes.xmax=xmax; bornes.ymin=ymin; bornes.ymax=ymax;
    bornes.resultat=0;
    return(bornes);

}

