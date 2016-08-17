/** \file lecture_grille.c */

#include "lecture_grille.h"
#include "outils.h"
#include "coordonnees.h"
#include <ctype.h>
/* pour la fonction abs() */
#include "stdlib.h"

/* zone lambert2e utile */
#define LONMIN -5.5
#define LONMAX 11
#define LATMIN 39
#define LATMAX 51.5


/* lit et analyse l'entête du fichier de données */
grille_t *read_header_data(FILE *fd){
    int i; // nb de données lues (par ligne)
    char buf_read[300]; //buffer de lecture
    int verbose=1; // variable de solution de facilité (à revoir)

    /*valeurs à retourner*/
    format_t format;
    int width, height; // surtout celles-ci
    float x1,x2,y1,y2,hmin,hmax;
    grille_t *retour;
    retour=malloc(sizeof(grille_t));
    retour->data=NULL;
    retour->type=INDEFINI;

    fprintf(stdout, "Lecture de l'entête du fichier source ...\n");

    fgets(buf_read, 255, fd);  /* type de données */
    if(!strncmp(buf_read,"DSAA",4)) {
        format=GRD; // formaté ASCII
    } else if (!strncmp(buf_read,"Grille",6)) {
        format=GRI; // binaire (entiers sur 4 octets)
    } else if (!strncmp(buf_read,"Grill2",6)) {
        format=GR2; // binaire (shorts sur 2 octets)
    } else if (!strncmp(buf_read,"ALT",3)) {
        format=ALT; // binaire (shorts sur 2 octets inversés)
    } else {
        fprintf(stderr, "Erreur : mauvais format de données : %s\n", buf_read);
        format=INCONNU;
        retour->width=0; retour->height=0;
        return(retour);
    }
    if (verbose)
        fprintf(stdout, "Format : %s", buf_read);

    fgets(buf_read, 255, fd);  /* lecture de la taille de la grille */
    i = sscanf(buf_read, "%d %d", &width, &height);
    if (i<2) {
        fprintf(stderr, "L'entête des données d'entrée a des valeurs largeur/hauteur incorrectes:\n  %s\n", buf_read);
        retour->width=0; retour->height=0;
        return(retour);
        //exit(-1);
    }
    if (verbose)
        fprintf(stdout, "largeur/hauteur: %d %d\n", width, height);

    fgets(buf_read, 255, fd);  /* reading w_data h_data */
    i = sscanf(buf_read, "%f %f", &x1, &x2);
    if (i<2) {
        fprintf(stderr, "Plage pour x incorrecte:\n  %s\n", buf_read);
        retour->width=0; retour->height=0;
        return(retour);
        //exit(-1);
    }
    if (verbose)
        fprintf(stdout, "plage pour x: %f %f\n", x1, x2);


    fgets(buf_read, 255, fd);  /* reading w_data h_data */
    i = sscanf(buf_read, "%f %f", &y1, &y2);
    if (i<2) {
        fprintf(stderr, "Plage pour y incorrecte:\n  %s\n", buf_read);
        retour->width=0; retour->height=0;
        return(retour);
        //exit(-1);
    }
    if (verbose)
        fprintf(stdout, "plage pour y: %f %f\n", y1, y2);

    fgets(buf_read, 255, fd);  /* reading w_data h_data */
    i = sscanf(buf_read, "%f %f", &hmin, &hmax);
    if (i<2) {
        fprintf(stderr, "Bornes des valeurs incorrectes:\n  %s\n", buf_read);
        retour->width=0; retour->height=0;
        return(retour);
        //exit(-1);
    }
    if (verbose)
            fprintf(stdout, "valeurs min et max: %f %f\n", hmin, hmax);


    retour->format=format;
    retour->width=width;
    retour->height=height;
    retour->x1=x1; retour->x2=x2;
    retour->y1=y1; retour->y2=y2;
    retour->hmin=hmin; retour->hmax=hmax;

    retour->longueur = ftell(fd);
    if (verbose) fprintf(stdout, "longueur de l'entête : %d\n\n", retour->longueur);

    return(retour);
}
/* fin de read_header_data */



/* lit une chaine ou une valeur */
/* renvoie -1 en cas d'echec? à revoir */
int read_data(FILE *fd, /*char *valeur,*/  format_t format){
/* format : DSAA -> lecture d'une chaine à convertir en entier
**          Grille -> lecture d'un entier en binaire */

    char c; // un caractère
    int k; //compteur
    int l=9; // longueur max de valeur : au dela on risque le dépassement de buffer
    int n; /* la valeur lue */
    short ns; /* lecture sur 2 octets */
    char valeur[10];
    //fprintf(stdout, "Lecture d'une donnée du fichier source ...\n");


    if(format==GRD){
        //fprintf(stderr, "Détection du format DSAA\n");
        // supression des espaces
        c=fgetc(fd);
        k=0;
        while(isspace(c)) c=fgetc(fd);

        // lecture d'une valeur
        while( (!isspace(c)) && (c!=EOF) && (k<=l-1) ) {
            if( (c<'0')&&(c>'9')&&(c!='.')&&(c!='-') ) { // caractère invalide
                //return(-1);
                fprintf(stderr, "Caractère invalide parmis les données : %c\n", c);
                exit(1);
            }
            valeur[k]=c;
            c=fgetc(fd);
            k++;
        }

        valeur[k]='\0';
        n=atoi(valeur);
    }
    else if(format==GRI) {
        //fprintf(stderr, "Grille\n");
        fread(&n, sizeof(int), 1, fd); }
    else if(format==GR2) {
        //fprintf(stderr, "Grille\n");
        fread(&ns, sizeof(short), 1, fd);
        n=(int) ns; }
    else  {
        fprintf(stderr, "Format inconnu -> sortie\n");
         exit(56);
        }

  return(n);
}





/* liberation de la taille de donnees */
void destroy_grille(grille_t *grille){
    if(grille==NULL) return;
    if(grille->data!=NULL) free(grille->data);
    grille->data=NULL;
    free(grille);
    grille=NULL;
}




/*--------------------------------------------------------*/
/* ESSAI POUR LA NOUVELLE FONCTION DE LECTURE */

/*
*
*   lecture en une seule fct
*
*
*/
/* lecture des données */
grille_t *lit_grille_entiere(char *fichier){
    return(lit_grille(fichier,-1,-1,-1,-1));
}

/* float x1 à y2 : fenetre d'interception souhaitée */
grille_t *lit_grille(char *fichier,float x1,float x2,float y1,float y2){
    FILE *fd; // descripteur de fichier

    grille_t *grille;

    //float x1, x2, y1, y2; //valeurs lambert utiles (zone des données à afficher (intersection avec) zone de la fenetre)
    int i1, i2, j1, j2; // correspondance en numéro de point de grille
    int t; //variable temporaire pour l'échange de valeurs
    //caractéristiques de la grille
    float xmax, ymax, xmin, ymin; // non utiles (pour l'instant) position du max et du min par rapport à la grille
    //char c; // un caractère
    int i, j;//, k; //nb de données lues (par ligne) - compteurs
    float xlamb, ylamb; //
    //char valeur[10]; // une des valeurs du fichier
    //int l=9; // longueur max de valeur : au dela on risque le dépassement de buffer
    int n; //valeur transformée en entier
    short ns; /* ou en short */
    char c1, c2; /* ou de 2 caracteres */
    //char buf_read[300]; //buffer de lecture
    //int verbose=0; // variable de solution de facilité (à revoir)


    if(!(fd=fopen(fichier, "r"))) {
        fprintf(stderr, "Ouverture de %s impossible!\n", fichier);
        return(NULL);
    }

    //lecture de l'entête
    grille=read_header_data(fd);
    //fin de lecture de l'entête
    //fprintf(stdout, "Entete de %s lue!\n", fichier);

    /* FENETRE D'INTERSECTION */
    if(x1<0) x1=grille->x1;
    if(x2<0) x2=grille->x2;
    if(y1<0) y1=grille->y1;
    if(y2<0) y2=grille->y2;

    if(x1>x2) { t=x1; x1=x2; x2=t; }
    if(y1>y2) { t=y1; y1=y2; y2=t; }

    if((grille->x2<x1)||(grille->x1>x2)||(grille->y1>y2)||(grille->y2<y1)){
        fprintf(stderr,"grille (%f:%f * %f:%f) incompatible avec la fenêtre d'intersection(%f:%f * %f:%f)\n", grille->x1, grille->x2, grille->y1, grille->y2, x1, x2, y1, y2);
        fprintf(stderr, "Aucune donnée lue\n");
        grille->width=0; grille->height=0;
        return(grille);
    }

    //zone utile
    x1=(grille->x1>x1)?grille->x1:x1; //maxi
    x2=(grille->x2<x2)?grille->x2:x2; //mini
    y1=(grille->y1>y1)?grille->y1:y1; //maxi
    y2=(grille->y2<y2)?grille->y2:y2; //mini


    //Non utilisation de la fenêtre (pour l'instant?)
    grille->data=malloc(sizeof(int)*grille->width*grille->height);
    //fprintf(stdout, "Allocation de %d octets\n", sizeof(int)*grille->width*grille->height);

    grille->type=INDEFINI;

    // lecture des données


    /* FORMAT GRD */
    if(grille->format==GRD){
        for(j=grille->height;j>=1;j--){ // on commence par la ligne du bas
            for(i=1; i<=grille->width; i++){

                n=read_data(fd, grille->format);

                xlamb=(500000*grille->x1+500000*i*(grille->x2-grille->x1)/grille->width)/500000;
                ylamb=(500000*grille->y1+500000*(grille->height-j)*(grille->y2-grille->y1)/grille->height)/500000;
                /*on peut gagner du temps si on est hors champ : */
                if( (xlamb>x1) && (xlamb<x2) && (ylamb>y1) && (ylamb<y2) ) {

                    if(n>grille->hmax) {
                        grille->hmax=n; xmax=i; ymax=j;
                    }
                    if(n<grille->hmin) {
                        grille->hmin=n; xmin=i; ymin=j;
                    }
                }


                setdata(grille, i, j, n);
                //grille->data[i][j]=n;

                /*if((c==EOF)&&(j>1)&&(i<grille->width)){
                    fprintf(stderr, "Fin inattendue du fichier %s\n", fichier);
                    return(8);
                }*/

            } // fin de for i
            // sortie de la fenêtre d'affichage
            if(ylamb>y2) break;
        } // fin de for j
    } // fin de if format grd


    else if(grille->format==GRI){
        i1=transforme_x(x1, grille->width, grille->x1, grille->x2);
        i2=transforme_x(x2, grille->width, grille->x1, grille->x2);
        j1=transforme_y(y1, grille->height, grille->y1, grille->y2);
        j2=transforme_y(y2, grille->height, grille->y1, grille->y2);
        /* attention j1 et j2 sont des coordonnées fenetre comptées à partir du haut */
        /* il faut les transformer en données grille pour la lecture puis repasser en coord fenetre pour l'affichage */

        printf("%d %d %d %d\n", i1, i2, j1, j2);
        fseek(fd, grille->longueur+(sizeof(int)*(grille->height-j1)*grille->width), SEEK_SET);

        for(j=j1;j>=j2;j--){ // on commence par la ligne du bas
            fseek(fd, sizeof(int)*(i1-1), SEEK_CUR);
            for(i=i1; i<=i2; i++){
                /*n=read_data(fd, grille->format); */ fread(&n, sizeof(int), 1, fd);
                setdata(grille, i, j, n);
                //grille->data[i][j]=n;
            }
            fseek(fd, sizeof(int)*(grille->width-i2), SEEK_CUR);
            //actualise_fenetre(f);
        }

    }// fin de if format GRI (binaire)


    else if(grille->format==GR2){
        i1=transforme_x(x1, grille->width /*f.w*/, grille->x1, grille->x2);
        i2=transforme_x(x2, grille->width /*f.w*/, grille->x1, grille->x2);
        j1=transforme_y(y1, grille->height /*f.h*/, grille->y1, grille->y2);
        j2=transforme_y(y2, grille->height /*f.h*/, grille->y1, grille->y2);
        /* attention j1 et j2 sont des coordonnées fenetre comptées à partir du haut */
        /* il faut les transformer en données grille pour la lecture puis repasser en coord fenetre pour l'affichage */


        printf("plages : x : %f %f; y : %f %f\n", x1, x2, y1, y2);
        printf("plages : i : %d %d; j : %d %d\n", i1, i2, j1, j2);

        fseek(fd, grille->longueur+(sizeof(short)*(grille->height-j1)*grille->width), SEEK_SET);

        for(j=j1;j>=j2;j--){ // on commence par la ligne du bas
            //printf("Parcours du fichier. j : %d - ", j);
            fseek(fd, sizeof(short)*(i1-1), SEEK_CUR);
            for(i=i1; i<=i2; i++){
                //printf("i : %d - ", i);
                /*n=read_data(fd, grille->format); */
                fread(&ns, sizeof(short), 1, fd);
                //printf("ns : %d\n", ns);
                n=(int) ns;

                setdata(grille, i, j, n);
                //grille->data[i][j]=n;
                //if(n>1000) printf("n=%d => %d\n",n, getdata(grille,i,j));
            }
            fseek(fd, sizeof(short)*(grille->width-i2), SEEK_CUR);
        }

    }// fin de if format GRD (binaire)



    else if(grille->format==ALT){
        i1=transforme_x(x1, grille->width, grille->x1, grille->x2);
        i2=transforme_x(x2, grille->width, grille->x1, grille->x2);
        j1=transforme_y(y1, grille->height, grille->y1, grille->y2);
        j2=transforme_y(y2, grille->height, grille->y1, grille->y2);
        /* attention j1 et j2 sont des coordonnées fenetre(taille grille pourtant) comptées à partir du haut */
        /* il faut les transformer en données grille pour la lecture puis repasser en coord fenetre pour l'affichage */

        printf("plages : x : %f %f; y : %f %f\n", x1, x2, y1, y2);
        printf("plages : i : %d %d; j : %d %d\n", i1, i2, j1, j2);
        fseek(fd, grille->longueur+(sizeof(short)*(/*grille->height-j1*/j2)*grille->width), SEEK_SET);


        for(j=j2;j<=j1;j++){ // on commence par la ligne du haut
            fseek(fd, sizeof(short)*(i1-1), SEEK_CUR);
            for(i=i1; i<=i2; i++){


                /*n=read_data(fd, grille->format); */
                /*fread(&ns, sizeof(short), 1, fd);
                n=(int) ns;*/


                /* ordre des octects inverses */
                //gzseek(gzd, 2*i+header_data, SEEK_SET);
                    //if (gzeof(gzd)) return NOVALUE;
                    c1 = fgetc(fd);
                    //if (gzeof(gzd)) return NOVALUE;
                    c2 = fgetc(fd);

                    n = (c1<<8) | c2;
                    // high bit might be "ocean bit"; next bit is, in any case, sign bit
                    n = n&0x7FFF;
                    if (n&0x4000) n |= 0xFFFF8000;

                    //if (n<min_alt) min_alt = n;
                    //if (n>max_alt) max_alt = n;
                    //in_ocean = c1>>7;

                //if((n>0)&&(n<8500)) printf("*%f ", (500000*grille->x1+500000*i*(grille->x2-grille->x1)/grille->width)/500000);

                if(n==-9999) n=9000;

                setdata(grille, i, j, n);
                //grille->data[i][j]=n;
            }
            fseek(fd, sizeof(short)*(grille->width-i2), SEEK_CUR);
        }

    }// fin de if format ALT (binaire)
    // fin de la lecture

    return(grille);
}

/* fct interne */
/* tranforme un indice bidimensionnel (i,j) en indice linéaire */
int getindice(grille_t *grille, int i, int j);

/* Met la valeur n, en position i,j dans les données */
int setdata(grille_t *grille, int i, int j, int n){
    if (grille==NULL) return(0);
    if ((grille->width<=i)||(grille->height<=j)) return(0);
    if (grille->data==NULL) return(0);

    grille->data[getindice(grille, i, j)]=n;
    return(n);
}

/* récupère la valeur en position i,j dans les données
** renvoie zéro si la position n'existe pas */
int getdata(grille_t *grille, int i, int j){
    if (grille==NULL) { fprintf(stderr,"Erreur : grille inexistante\n"); return(0);}
    if ((grille->width<=i)||(grille->height<=j)) { /*fprintf(stderr,"Erreur : demande hors champ : (i,j)=%d,%d (w,h)=%d,%d\n", i,j, grille->width, grille->height);*/ return(0);}
    if (grille->data==NULL) { fprintf(stderr,"Erreur : données inexistantes\n"); return(0); }

    return(grille->data[getindice(grille, i, j)]);
}

/* fct interne */
/* tranforme un indice bidimensionnel (i,j) en indice linéaire */
int getindice(grille_t *grille, int i, int j){
    return(i+j*grille->width);
}



/* transformation de grille LATLON en LAMBERT2E */
grille_t *transforme_vers_Lambert(grille_t *grille_latlon){

    grille_t *retour; /* la grille lambert */
    float latmin, latmax, lonmin, lonmax; /* zone concernée en latlon */
    coord_geo point_geo; /* coordonnées latlon à transformer en lambert */
    coord_lamb point_lamb; /* le resultat de la transformation */
    int i,j; /* compteurs */
    int i1,i2,j1,j2; /* bornes */
    float deltaX, deltaY; /* résolution de la grille lambert */


    /* sorties en echec */
    if(grille_latlon->type != LATLON) return NULL;
    if(grille_latlon->data == NULL) return NULL;

    /* zone utile */
    lonmin=(LONMIN > grille_latlon->x1 ? LONMIN: grille_latlon->x1);
    lonmax=(LONMAX < grille_latlon->x2 ? LONMAX: grille_latlon->x2);
    latmin=(LATMIN > grille_latlon->y1 ? LATMIN: grille_latlon->y1);
    latmax=(LATMAX < grille_latlon->y2 ? LATMAX: grille_latlon->y2);

    if((lonmin>=lonmax)||(latmin>=latmax)) return NULL;
    printf("Zone geo à transformer en Lambert : (%.0f°,%.0f°) lon * (%.0f°,%.0f°) lat\n",lonmin, lonmax, latmin,latmax);

    /* allocation de la nouvelle grille */
    retour=malloc(sizeof(grille_t));
    retour->longueur=grille_latlon->longueur;
    retour->type=LAMBERT2E;
    retour->format=INCONNU;
    retour->hmin=0;
    retour->hmax=0;

    i1=transforme_x(lonmin,grille_latlon->width,grille_latlon->x1, grille_latlon->x2);
    i2=transforme_x(lonmax,grille_latlon->width,grille_latlon->x1, grille_latlon->x2);
    /* Attention, les j sont comptes a partir du haut */
    j1=transforme_y(latmin,grille_latlon->height,grille_latlon->y1, grille_latlon->y2);
    j2=transforme_y(latmax,grille_latlon->height,grille_latlon->y1, grille_latlon->y2);
    /* recentrage des bornes par rapport au centre des pixels */
    lonmin=transforme_i(i1,grille_latlon->width,grille_latlon->x1, grille_latlon->x2);
    lonmax=transforme_i(i2,grille_latlon->width,grille_latlon->x1, grille_latlon->x2);
    latmin=transforme_j(j1,grille_latlon->height,grille_latlon->y1, grille_latlon->y2);
    latmax=transforme_j(j2,grille_latlon->height,grille_latlon->y1, grille_latlon->y2);

    fprintf(stdout,"point de la grille d'origine concernés : (%d;%d) * (%d;%d)\n",i1,i2,j2,j1);

    /* correspondance des bornes en Lambert */
    point_geo.lambda=lonmin;
    point_geo.phi=latmin;
    point_lamb=calcule_Lambert(point_geo);
    retour->x1=point_lamb.X/100.; /* transformation des mètres en hectomètres */
    retour->y1=point_lamb.Y/100.;
    point_geo.lambda=lonmax;
    point_geo.phi=latmax;
    point_lamb=calcule_Lambert(point_geo);
    retour->x2=point_lamb.X/100.;
    retour->y2=point_lamb.Y/100.;


    /* Conservation de la résolution avant élargissement de la grille */
    deltaX=(retour->x2 - retour->x1)/(i2 - i1);
    deltaY=(retour->y2 - retour->y1)/(j1 - j2); /* j1 > j2 */

    /* Elargissement vers le Nord */
    point_geo.lambda=lonmin;
    point_geo.phi=latmax;
    point_lamb=calcule_Lambert(point_geo);
    if(retour->y2<point_lamb.Y/100.) retour->y2=point_lamb.Y/100.;
    /* Elargissement vers l'Est */
    point_geo.lambda=lonmax;
    point_geo.phi=latmin;
    point_lamb=calcule_Lambert(point_geo);
    retour->x2=point_lamb.X/100.;
    /* Elargissement vers le Sud */
    point_geo.lambda=(lonmax+lonmin)/2;
    point_geo.phi=latmin;
    point_lamb=calcule_Lambert(point_geo);
    if(retour->y1>point_lamb.Y/100.) retour->y1=point_lamb.Y/100.;

    fprintf(stdout,"Zone Lambert de la nouvelle grille : (%.0f;%.0f) * (%.0f;%.0f)\n",retour->x1,retour->x2,retour->y1,retour->y2);

    /* taille de la grille */
    retour->width=/*800;*/(int) ((retour->x2 - retour->x1)/deltaX) + 1;
    retour->height=/*640;*/(int) ((retour->y2 - retour->y1)/deltaY) + 1;
    fprintf(stdin,"Taille de la nouvelle grille : %d * %d\n",retour->width, retour->height);
    //return(NULL);
    retour->data=(int *) malloc(sizeof(int)*retour->width*retour->height);
    if(retour->data==NULL) {
        fprintf(stderr,"Echec d'allocation mémoire\n");
        free(retour);
        return(NULL);
    }


    /* initialisation */
    for(i=0;i<retour->width;i++) for(j=0;j<retour->height;j++) {
        setdata(retour, i, j, -9999);
    }

    /* remplissage de la grille */
    for(i=i1;i<=i2;i++) for(j=j2;j<=j1;j++) {

        int k, l; /* position dans la nouvelle grille */

        /* coordonnées géo du point courant */
        point_geo.lambda=transforme_i(i, grille_latlon->width,grille_latlon->x1, grille_latlon->x2);
        point_geo.phi=transforme_j(j, grille_latlon->height,grille_latlon->y1, grille_latlon->y2);
        /* Transformation en point lamb */
        point_lamb=calcule_Lambert(point_geo);
        /* calcul de la position dans la nouvelle grille */
        k=transforme_x(point_lamb.X/100.,retour->width,retour->x1, retour->x2);
        l=transforme_y(point_lamb.Y/100.,retour->height,retour->y1, retour->y2);
        /* mise en place des données */
        setdata(retour, k, l, getdata(grille_latlon,i,j)/*>0?getdata(grille_latlon,i,j):0*/);
        if(retour->hmin > getdata(retour,k,l)) retour->hmin=getdata(retour,k,l);
        if(retour->hmax < getdata(retour,k,l)) retour->hmax=getdata(retour,k,l);
    }

    return(retour);
}

void info_grille(grille_t *grille){
    fprintf(stdout, "\nInformation sur la grille :\n");
    if(grille==NULL){
        fprintf(stdout, "Attention grille non allouée\n");
        return;
    }
    fprintf(stdout, "Format = %d\n", grille->format);
    fprintf(stdout, "Longueur * Hauteur : %d * %d\n", grille->width, grille->height);
    fprintf(stdout, "Bornes de la zone : (%.0f,%.0f) * (%.0f,%.0f)\n",grille->x1,grille->x2,grille->y1,grille->y2);
    fprintf(stdout, "Type de coordonnées : %d\n", grille->type);
}

