/** \file affichage.c */


#include <math.h>
#include "affichage.h"
#include "lecture_grille.h"



/** Array of the pixels with usefull color. Is used in relief grid. */
Pixel *pix = NULL;

/** Max number of pixels in 'pix' array */
#define PLAGE 12000



/* Initialisation de la palette de couleurs */
/* la palette "range" est passée en paramètre */
/* initialisation des tableaux de 24000 valeurs : red, green, blue ainsi que pix(le tableau de Pixels XWindows)*/
static void init_colors(char *range, int depth, int gamma_f, int hmax)
{
    int i, j, k, boo; // j:valeur de niveau, i:valeur du précédent niveau, k:valeur intermédaire pour interpolation,  boo:couleur associée au niveau j trouvée
    char *ptr1, *ptr2; // chaines extraites de "range"
    unsigned int col; // couleur correspondant au niveau "j"
    unsigned char c1, c2, c3; // couleurs RGB du précendent niveau
    unsigned char u, v; // utilisés pour calculer la couleur du pixel lorsque "depth" != 24
    double f; // facteur pour l'interpolation
    int verbose=0;
    unsigned char *red, *green, *blue;
    red=NULL; green=NULL; blue=NULL;


    if(hmax<200) hmax=0;

    if (!range) {
        //range=color_scheme;
        return;
    }

    if (verbose) fprintf(stderr, "Computing color range ...\n");

    ptr1 = ptr2 = range;
    if (!red) red = (unsigned char *)malloc(24002*sizeof(unsigned char));
    if (!green) green = (unsigned char *)malloc(24002*sizeof(unsigned char));
    if (!blue) blue = (unsigned char *)malloc(24002*sizeof(unsigned char));

    i = -1;
    boo = 1;

  begrange:
    while (*ptr2 && *ptr2!=',') ++ptr2;
    if (*ptr2==',') {
        *ptr2 = '\0';
        j = atoi(ptr1)+12000;
    } else // on a atteind la fin de la chaine "range"
        j = -1;

    if (j<0 || j>24000) {
        fprintf(stderr, "Erroneous color range !!\n\n");
        exit(-1);
    }
    ++ptr2;
    ptr1 = ptr2;
    while (*ptr2 && *ptr2!=',') ++ptr2;
    if (!*ptr2) boo=0; // pas de couleur associée
    *ptr2 = '\0';
    sscanf(ptr1, "%x", &col);

    //adaptation de la palette
    if((hmax>0)&&(hmax<5000)&&(j>12001)/*sinon arrondit -> mer en vert*/)j=((j-12000)*hmax/5000)+12000;


    red[j] = col>>16;
    green[j] = (col>>8)&255;
    blue[j] = col&255;
    if (i==-1) {
        c1 = red[j];
        c2 = green[j];
        c3 = blue[j];
    } else {
        c1 = red[i];
        c2 = green[i];
        c3 = blue[i];
    }

    //printf("%d : %c %c %c\n", j, c1, c2, c3);
    // on a remplit le tableau au niveau j
    // maintenant on interpole entre i et j;
    for (k=i+1; k<j; k++) {
        f = ((double)(k-i))/((double)(j-i));
        red[k] = (unsigned char) ((1.0-f)*((double)c1)+f*((double)red[j])+0.5);
        green[k] = (unsigned char) ((1.0-f)*((double)c2)+f*((double)green[j])+0.5);
        blue[k] = (unsigned char) ((1.0-f)*((double)c3)+f*((double)blue[j])+0.5);
    }
    if (boo) {
        ++ptr2;
        ptr1 = ptr2;
        i = j;
        goto begrange;
    }
    for (k=j+1; k<=24000; k++) {
        red[k] = red[j];
        green[k] = green[j];
        blue[k] = blue[j];
    }
    if (gamma_f<1.0) {
        for (k=0; k<=24000; k++) {
            red[k] = (unsigned char) (gamma_f*red[k]+0.5);
            green[k] = (unsigned char) (gamma_f*green[k]+0.5);
            blue[k] = (unsigned char) (gamma_f*blue[k]+0.5);
        }
    } else
    if (gamma_f>1.0) {
        gamma_f = gamma_f - 1.0;
        for (k=0; k<=24000; k++) {
            red[k] = (unsigned char) ((1.0-gamma_f)*red[k]+gamma_f*255.0+0.5);
            green[k] = (unsigned char) ((1.0-gamma_f)*green[k]+gamma_f*255.0+0.5);
            blue[k] = (unsigned char) ((1.0-gamma_f)*blue[k]+gamma_f*255.0+0.5);
        }
        gamma_f = gamma_f + 1.0;
    }
    if (verbose)
        fprintf(stderr, "colors[-12000...12000] written to palette.\n\n");

    if (!pix)
        pix = (Pixel *)malloc(24002*sizeof(Pixel));
    if (verbose)
        fprintf(stderr, "Indexing colors for the X Window system\n");
    for (k=0; k<=24000; k++) {
        if (verbose && k%500==0) {
            fprintf(stderr, "[%d]", k);
            if ((k%500)%10==9)
                fprintf(stderr, "\n");
            else
                fprintf(stderr, " ");
        }
        if (depth>=24) {
            pix[k] = (red[k]<<16) | (green[k]<<8) | blue[k];
        } else
        if (depth==16) {
            u = (blue[k]&248)>>3 | (green[k]&28)<<3;
            v = (green[k]&224)>>5 | (red[k]&248);
            pix[k] = (v<<8)|u;
        } else
        if (depth==15) {
            u = (blue[k]&248)>>3 | (green[k]&56)<<2;
            v = (green[k]&192)>>6 | (red[k]&248)>>1;
            pix[k] = (v<<8)|u;
        }
    }
    if (verbose) fprintf(stderr, "\n\n");

    /* free color buffers */
    free(red); free(green); free(blue);
}
/* end of init_colors */







int affiche_grille(fenetre f, char *fichier, char *palette, char *color_scheme, int depth, int hmax){

    grille_t *grille;
    int retour;

    grille=lit_grille_entiere(fichier);

    retour=affiche_grille_(f, grille, palette, color_scheme, depth, hmax);
    destroy_grille(grille);
    return(retour);
}



int affiche_grille_(fenetre f, grille_t *grille, char *palette, char *color_scheme, int depth, int hmax){

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

    dxFen=(f.x2-f.x1)/(f.window.w-1); dyFen=(f.y2-f.y1)/(f.window.h-1);
    dxGrille=(grille->x2-grille->x1)/(grille->width-1); dyGrille=(grille->y2-grille->y1)/(grille->height-1);
    largeurX=floor(dxGrille/(dxFen*2));
    largeurY=floor(dyGrille/(dyFen*2));
    //largeur=(int) ((largeurX+largeurY)/2);
    largeur=(largeurX>largeurY)?largeurX:largeurY;

    // pointage dans la fenêtre

    if(pix == NULL) {
        init_colors(color_scheme, depth, 1, hmax);
    }
    
    fprintf(stdout,"pointage des points (i,j) (%d:%d , %d:%d)\n", i1, i2, j2, j1);
    for(j=j2; j<=j1; j++){
        for(i=i1; i<=i2; i++){

            //n=grille->data[i][j];
            n=getdata(grille, i, j);
            //if(n>0) printf("%d,%d -> %d\n",i,j, n);
            if(n>PLAGE) n=PLAGE;
            if(n<-PLAGE) n=-PLAGE; //evitons les dépassements de mémoire
            if (n>0){

                //xlamb=(500000*grille->x1+500000*i*(grille->x2-grille->x1)/grille->width)/500000;
                //ylamb=(500000*grille->y1+500000*(grille->height-j)*(grille->y2-grille->y1)/grille->height)/500000;
                xlamb=transforme_i(i, grille->width, grille->x1, grille->x2);
                ylamb=transforme_j(j, grille->height, grille->y1, grille->y2);

                pointe_pixel(f, xlamb, ylamb, largeur, pix[n+12000], CARRE);
                //printf("%d ",n);
            }


        } // fin de for i

        // show the line
        actualise_fenetre(f.window);

    } // end of for j

    actualise_fenetre(f.window);

    // free pix
    free(pix);
    pix=NULL;

    return(0);
}

