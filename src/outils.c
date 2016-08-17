/** \file outils.c */

#include "outils.h"

#include <math.h>

/* transforme les coordonnées planes (Lambert, lon-lat, ...) en coordonnées fenetre ou grille */
/* les coordonnées trouvées peuvent etre a l'exterieur de la fenetre    */
/* les valeurs x1 et x2 correspondent au milieu des pixels 1 et w */
int transforme_x(float xLamb, int w, float x1, float x2){
    float d; // Delta xLamb entre 2 pixels
    float e; // erreur entre xlamb réel et xlamb pixel
    float t; // auxiliaire temporaire
    int i; // valeur de retour

    /*if((xLamb<x1)||(xLamb>x2)) return(-1);*/
    /*if(xLamb<=x1) return(0);
    if(xLamb>=x2) return(w);*/

    d=(x2-x1)/(w-1);
    t=(xLamb-x1)/d;
    i=1+t;
    e=xLamb-(x1+(i-1)*d);
    if(e>d/2) i++; else if(e<-d/2) i--;

    return(i);
}

int transforme_y(float yLamb, int h, float y1, float y2) {
    float d; // Delta yLamb entre 2 pixels
    float e; // erreur entre ylamb réel et ylamb pixel
    float t; // auxiliaire temporaire
    int j; // valeur de retour

    /*if((yLamb<y1)||(yLamb>y2)) return(-1);*/
    /*if(yLamb<=y1) return(h);
    if(yLamb>=y2) return(0);*/

    d=(y2-y1)/(h-1);
    t=(yLamb-y1)/d;
    j=1+t;
    e=yLamb-(y1+(j-1)*d);
    if(e>d/2) j++; else if(e<-d/2) j--;
    return(h-j+1); // y fenêtre compté à partir du haut

}




float transforme_i(int i, int w, float x1, float x2){
    //return((500000*x1+500000*(i-1)*(x2-x1)/(w-1))/500000);
    return(x1+(i-1)*(x2-x1)/(w-1));
}
float transforme_j(int j, int h, float y1, float y2){
    //return((500000*y1+500000*(h-j)*(y2-y1)/(h-1))/500000);
    return(y1+(h-j)*(y2-y1)/(h-1));
}
