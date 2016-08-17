/** \file affichage.c */


#include "affichage.h"
#include "lecture_grille.h"
/*
*
*   lecture en une seule fct
*   et affichage dans la fenetre f
*
*/
int affiche_grille(fenetre f, char *fichier, char *palette){

    grille_t *grille;
    int retour;

    grille=lit_grille_entiere(fichier);

    retour=affiche_grille_(f, grille, palette);
    destroy_grille(grille);
    return(retour);
}




/* Affichage des données dans une fenetre */
int affiche_grille_(fenetre f, grille_t *grille, char *palette){

    float x1, x2, y1, y2; //coordonees de la zone utile (données à afficher (intersection avec) zone de la fenetre)
    int i1, i2, j1, j2; // correspondance en numéro de point de grille
    int i, j; //compteurs
    float xlamb, ylamb; // transformation de i et j
    int n; //valeur à pointer = grille->data[i][j]
    float dxFen, dyFen; // resolution de la fenetre
    float dxGrille, dyGrille; // resolution de la grille
    int largeurX, largeurY, largeur; // demi-taille du point de grille dans la fenetre


    if((grille->x2<f.x1)||(grille->x1>f.x2)||(grille->y1>f.y2)||(grille->y2<f.y1)){
        fprintf(stderr,"grille (%f:%f * %f:%f) incompatible avec la fenêtre (%f:%f * %f:%f)\n", grille->x1, grille->x2, grille->y1, grille->y2, f.x1, f.x2, f.y1, f.y2);
        return(9);
    }

    //zone utile
    x1=(grille->x1>f.x1)?grille->x1:f.x1; //maxi
    x2=(grille->x2<f.x2)?grille->x2:f.x2; //mini
    y1=(grille->y1>f.y1)?grille->y1:f.y1; //maxi
    y2=(grille->y2<f.y2)?grille->y2:f.y2; //mini

    fprintf(stdout,"Affichage sur la zone (%f:%f * %f:%f)\n", x1, x2, y1, y2);
    fprintf(stdout,"intersection de la grille (%f:%f * %f:%f) avec la fenêtre (%f:%f * %f:%f)\n", grille->x1, grille->x2, grille->y1, grille->y2, f.x1, f.x2, f.y1, f.y2);

    i1=transforme_x(x1, grille->width, grille->x1, grille->x2);
    i2=transforme_x(x2, grille->width, grille->x1, grille->x2);
    j1=transforme_y(y1, grille->height, grille->y1, grille->y2);
    j2=transforme_y(y2, grille->height, grille->y1, grille->y2);

    dxFen=(f.x2-f.x1)/(f.w-1); dyFen=(f.y2-f.y1)/(f.h-1);
    dxGrille=(grille->x2-grille->x1)/(grille->width-1); dyGrille=(grille->y2-grille->y1)/(grille->height-1);
    largeurX=floor(dxGrille/(dxFen*2));
    largeurY=floor(dyGrille/(dyFen*2));
    //largeur=(int) ((largeurX+largeurY)/2);
    largeur=(largeurX>largeurY)?largeurX:largeurY;

    // pointage dans la fenêtre
    fprintf(stdout,"pointage des points (i,j) (%d:%d , %d:%d)\n", i1, i2, j2, j1);
    for(j=j2; j<=j1; j++){
        for(i=i1; i<=i2; i++){

            //n=grille->data[i][j];
            n=getdata(grille, i, j);
            //if(n>0) printf("%d,%d -> %d\n",i,j, n);
            if(n>PLAGE) n=PLAGE; if(n<-PLAGE) n=-PLAGE; //evitons les dépassements de mémoire
                if (n>0){

                    /*xlamb=(500000*grille->x1+500000*i*(grille->x2-grille->x1)/grille->width)/500000;
                    ylamb=(500000*grille->y1+500000*(grille->height-j)*(grille->y2-grille->y1)/grille->height)/500000;*/
                    xlamb=transforme_i(i, grille->width, grille->x1, grille->x2);
                    ylamb=transforme_j(j, grille->height, grille->y1, grille->y2);

                    pointe_pixel(f, xlamb, ylamb, /*0*/largeur, pix[n+12000], CARRE);
                    //printf("%d ",n);
                }


        } // fin de for i

        /* affichage de la ligne */
        actualise_fenetre(f);

    } // fin de for j

    actualise_fenetre(f);

    return(0);
}

