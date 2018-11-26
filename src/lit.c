/** \file lit.c */

/* programme de test de la bibliothèque de lecture des shapefiles */


#include "shape_data.h"


/* pour le tracé */
#include "fenetre.h"
#include "outils.h"


#define FICDATA "../../donnees_carto/communes/commune.shp"
#define XMIN 0
#define XMAX 1200000
#define YMIN 1600000
#define YMAX 2800000
/*#define XMIN 698000
#define XMAX 700000
#define YMIN 2499000
#define YMAX 2500000*/
/*#define XMIN 427000
#define XMAX 430000
#define YMIN 1951000
#define YMAX 1954000*/



/* tracé d'une forme générique */
int trace_shape(fenetre f, GenericShape *sh){

    if(sh==NULL) return 1;

    if((sh->shapeType==POLYGON) || (sh->shapeType==POLYLINE)){

        int N; /*nb de points*/
        int Np; /* nb de parties */
        int i; /* compteur */

        N=sh->numPoints;
        
        float *X = malloc(N*sizeof(float));
        float *Y = malloc(N*sizeof(float));

        if(X==NULL || Y==NULL) {
            if(X!=NULL) free(X);
            if(Y!=NULL) free(Y);
            return 2;
        }

        for(i=0; i<=N-1; i++){
            X[i] = sh->points[i].x;
            Y[i] = sh->points[i].y;
        }

        Np=sh->numParts;
        for(i=0; i<=Np-2; i++){
            int n = sh->parts[i+1]-sh->parts[i];
            if(sh->shapeType==POLYGON)
                window_fill_polygon(f, X+sh->parts[i], Y+sh->parts[i], n, ROUGE); 
            window_draw_lines(f, X+sh->parts[i], Y+sh->parts[i], n, JAUNE, 1);
        }
        int n = N-sh->parts[Np-1];
        if(sh->shapeType==POLYGON)
            window_fill_polygon(f, X+sh->parts[Np-1], Y+sh->parts[Np-1], n, ROUGE);
        window_draw_lines(f, X+sh->parts[Np-1], Y+sh->parts[Np-1], n, JAUNE, 1);

        free(X);
        free(Y);
    } /*if(sh->shapeType==POLYGON)*/
    else {
        fprintf(stderr, "Non supported shape : %d\n", sh->shapeType);
    }

    return(0);
}



/* tracé sur une fenetre des données */
int trace_e(Enregistrement *e){
    int width, height; // taille de la fenetre
    fenetre f; // fenetre d'affichage du tracé
    int depth; // nb de couleurs?
    XEvent event; //evenement clavier ou souris
    int i; //numero d'evenement clavier

    /* Création de la fenêtre */
    init_Xvariable();
    width=1000; height=800;
    //depth=256;
    //f=cree_fenetre_coloree(width, height, &depth, bornes.xmin, bornes.xmax, bornes.ymin, bornes.ymax, BLEU);
    f=cree_fenetre_coloree(width, height, &depth, XMIN, XMAX, YMIN, YMAX, BLEU);

    /* FONCTION DE TRACES */
    while (e!=NULL) {
        //printf("%d ",e->numero);
        if (e->sh!=NULL) trace_shape(f, e->sh);
        e=e->next;
    }
    /* TRACES EFFECTUES */



    /* Boucle principale*/
    while(1) {
        XNextEvent(f.window.dpy, &event);
        i = manage_event(f.window, event);
        if (i==2) break;
    }

    /* fermeture des display */
    fermeture(f);
    return 0;
}




/* PROGRAMME PRINCIPAL */
int main(int argc, char **argv){

    char *fichier; /* les données à lire */
    FILE *fd; /* pour ouvrir le fichier */

    Enregistrement *enr; /*la liste des enregistrement décodés */
    shape_header *entete; /* les infos d'entete */

    /* lecture des arguments */
    if(argc >=2) fichier=argv[1];
    else fichier=FICDATA;

    /* programme (boucle) principal(e) */

    /* présentation */
    printf("%s : lecture de données SHP\n", argv[0]);
    printf("Utilisation du fichier %s\n\n", fichier);

    /* ouverture du fichier */
    fd=fopen(fichier, "rb");
    if(fd==NULL) {
        printf("Impossible d'ouvrir le fichier %s\n\n", fichier);
        return(1);
    }
    fprintf(stdout, "Fichier %s ouvert\n\n", fichier);

    /* entete */
    entete=lit_entete(fd);
    if(entete==NULL){
        printf("Entete invalide\n\n");
        return(2);
    }
    fprintf(stdout, "Contenu de l'entête :\n");
    affiche_entete(entete);
    fprintf(stdout, "\n");

    /* enregistrements */
    enr=lecture_fichier(fd, entete);

    /**/
    fclose(fd);

     /**/
    if(enr==NULL){
        printf("Aucun enregistrement lu\n");
        return(3);
    }
    trace_e(enr);
    printf("\nFIN\n");


    /* libération de la mémoire */
    detruit_entete(entete);
    detruit_enregistrement(enr);
    return(0);
}

