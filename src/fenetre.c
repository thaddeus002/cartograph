/**
 * \file fenetre.c
 * Management of a window and the main functions of the xlib.
 */

#include "fenetre.h"

#include <math.h>
/** calculate x point from coordinate */
int xpoint(fenetre *f, float a) {
    return transforme_x(a, f->window.w, f->x1, f->x2);
}

/** calculate y point from coordinate */
int ypoint(fenetre *f, float b) {
    return transforme_y(b, f->window.h, f->y1, f->y2);
}



/**
 * \return w=0 if fail
 */
fenetre cree_fenetre_coloree(int width, int height, int *depth, float x1, float x2, float y1, float y2, couleur c){

    fenetre retour;
    retour.window = create_window_with_background_color(width, height, depth, c);
    retour.gs = EPSG_4326; //default value
    retour.x1=x1; retour.x2=x2; retour.y1=y1; retour.y2=y2;

    return(retour);
}


/* retourne w=0 si echec */
fenetre cree_fenetre(int width, int height, int *depth, float x1, float x2, float y1, float y2){
    return(cree_fenetre_coloree(width, height, depth, x1, x2, y1, y2, NOIR));
}

/* trace un point en fct des coordonnées Lambert */
int pointe_pixel(fenetre f, float x, float y, int L, /*couleur c*/Pixel p, forme_t forme) {
    int i, j; // les coordonnées sur la carte
    int k, l; //compteurs
    //int L=1; //demi largeur du point

    /* passage des coordonnées cartes aux coordonnées fenêtre */
    i=transforme_x(x, f.window.w, f.x1, f.x2)-1; /* les coord fenetre vont de 0 à w-1 */
    j=transforme_y(y, f.window.h, f.y1, f.y2)-1;

    if(i<0) return(1);
    if(j<0) return(1);
    if((i>f.window.w)||(j>f.window.h)) return(2);

    if(L==0) forme=CARRE;
    if (forme==CARRE) // carré
        for(k=-L; k<=L; k++)
            for(l=-L; l<=L; l++)
                do_output_pixel(f.window,i+l,j+k, p);
    else if(forme==TRIANGLE_) // triangle
        for(k=-L; k<=L+1; k++)
            for(l=-(k+L)*2/3; l<=(k+L)*2/3; l++)
                do_output_pixel(f.window,i+l,j+k, p);
    else if(forme==ROND) //rond
        for(k=-L; k<=L; k++)
            for(l=-floor(sqrt(L*L*10000-10000*k*k)/100); l<floor(sqrt(10000*L*L-10000*k*k)/100); l++)
                do_output_pixel(f.window,i+l,j+k, p);

    /* affichage du résultat dans la fenetre */
    //actualise_fenetre(f); //trop lent si on le fait pour chaque point
    return(0);
}


int pointe(fenetre f, float x, float y, int L, couleur c, forme_t forme) {
    return(pointe_pixel(f, x, y, L, pix_colore[c], forme));
}


/* met une échelle sur la carte */
/* seulement sur la largeur */
void trace_echelle(fenetre fen, couleur cdessin, couleur ctexte){
    int largeur; // taille de l'echelle
    int u, v; //position de l'echelle
    int x, h, n; // valeurs temporaires pour les calculs
    int terrain; // longueur sur le terrain
    char etiquette[50]; //valeur affichée
    char unite[3]; //mètres ou kilomètres

    if(fen.window.w<300) return; //trop petite fenetre
    if(fen.window.h<100) return;
    largeur = fen.window.w/10; // taille de base de l'échelle

    terrain=(fen.x2-fen.x1)*largeur/fen.window.w; // en hectomètres

    //arrondit de la largeur, ex: 12->10, 29->30
    n=0;
    x=terrain;
    while(x>10) {
        x=x/10; n++;
    }
    h=x+1;
    while(n>0){
        x=x*10; h=h*10; n--;
    }
    if(h-terrain<terrain-x) terrain=h;
    else terrain=x;

    /* calcul de la nouvelle largeur, après arrondit de la valeur de "terrain" */
    largeur=terrain*fen.window.w/(fen.x2-fen.x1);

    /* calcul de l'étiquette */
    if (terrain >= 40) {
        terrain=terrain/10;
        sprintf(unite,"km");
    } else {
        terrain=terrain*100;
        sprintf(unite,"m");
    }
    sprintf(etiquette, "%d %s", terrain, unite);

    u=fen.window.w-largeur*12/10;
    v=fen.window.h-15;

    XSetForeground(fen.window.dpy, fen.window.gc, pix_colore[cdessin]);
    XSetLineAttributes(fen.window.dpy, fen.window.gc, 0, LineSolid, CapRound, JoinMiter);
    XDrawLine(fen.window.dpy, fen.window.pixmap, fen.window.gc, u, v, u+largeur, v);
    XDrawLine(fen.window.dpy, fen.window.pixmap, fen.window.gc, u, v+1, u+largeur, v+1);
    XDrawLine(fen.window.dpy, fen.window.pixmap, fen.window.gc, u, v+2, u+largeur, v+2);
    XDrawLine(fen.window.dpy, fen.window.pixmap, fen.window.gc, u, v-1, u, v-4);
    XDrawLine(fen.window.dpy, fen.window.pixmap, fen.window.gc, u+1, v-1, u+1, v-4);
    XDrawLine(fen.window.dpy, fen.window.pixmap, fen.window.gc, u+largeur, v-1, u+largeur, v-4);
    XDrawLine(fen.window.dpy, fen.window.pixmap, fen.window.gc, u+largeur-1, v-1, u+largeur-1, v-4);
    display_text(fen.window, u+(largeur-10*strlen(etiquette))/2, v-7, etiquette, ctexte);

    /* affichage du résultat dans la fenetre */
    actualise_fenetre(fen.window);
}
/* fin de la fonction trace_echelle */

/* fermeture des display */
int fermeture(fenetre fen){
    /* fermeture de la fenetre */
    return(close_window(fen.window));
}


/**
 * Draw lines on a window.
 *
 * \param f the window where draw
 * \param x an array of X coordinates
 * \param y an array of Y coordinates
 * \param nb the number of points(x,y)
 * \param c lines' color
 * \param w lines' width
 * \return 0 on success or an error code
 */
int window_draw_lines(fenetre f, float *x, float *y, int nb, couleur c, int w) {

    XPoint *points;
    int i;

    points=malloc(nb*sizeof(XPoint));
    if(points==NULL){
        fprintf(stderr, "Could not allocate memory for draw lines\n");
        return 3;
    }

    for(i=0; i<nb; i++) {
        points[i].x=xpoint(&f, x[i]);
        points[i].y=ypoint(&f, y[i]);
    }

    trace_lignes(f.window, points, nb, c, w);
    free(points);
    return 0;
}

/**
 * Draw a filled polygon on a window.
 *
 * \param f the window where draw
 * \param x an array of X coordinates
 * \param y an array of Y coordinates
 * \param nb the number of points(x,y)
 * \param c polygon's color
 * \return 0 on success or an error code
 */
int window_fill_polygon(fenetre f, float *x, float *y, int nb, couleur c) {

    XPoint *points;
    int i;

    points=malloc(nb*sizeof(XPoint));
    if(points==NULL){
        fprintf(stderr, "Could not allocate memory for draw polygon\n");
        return 3;
    }

    for(i=0; i<nb; i++) {
        points[i].x=xpoint(&f, x[i]);
        points[i].y=ypoint(&f, y[i]);
    }

    remplit(f.window, points, nb, c);
    free(points);
    return 0;
}

