/**
 * \file window.c
 * Management of a window and the main functions of the xlib.
 */

#include "window.h"

/* to write in a file */
#include "yXinterface.h"
#include "yImage.h"
#include "yImage_io.h"


/* XSystem's variables */
Display * dpy;
GC gc;

XGCValues gcv;

Atom wm_delete_window, wm_protocols;
XFontStruct *xfont;

Window root, win;


/** Initialization of global variables. */
void init_Xvariable(){
    dpy=NULL;
}



/* retourne w=0 si echec */
window_t create_window_with_background_color(int width, int height, int *depth, couleur c){
    int i, j; //compteurs
    window_t retour;
    int scr;
    Visual * visual;

    if (dpy==NULL) {
        dpy = XOpenDisplay(NULL);
        if(dpy==NULL) {
            fprintf(stderr, "Could not open display");
            exit(1);
        }
        scr = DefaultScreen(dpy);
        visual = DefaultVisual(dpy, scr);
        root = DefaultRootWindow(dpy);

        pix_colore[NOIR] = BlackPixel(dpy, scr);
        pix_colore[BLANC] = WhitePixel(dpy, scr);
        pix_colore[ROUGE] = (250<<16) | (0<<8) | 0;
        pix_colore[VERT] = (0<<16) | (250<<8) | 0;
        pix_colore[BLEU] = (0<<16) | (0<<8) | 250;
        pix_colore[ORANGE_] = (250<<16) | (150<<8) | 0;
        pix_colore[JAUNE] = (250<<16) | (250<<8) | 0;
        pix_colore[CYAN_] = (0<<16) | (250<<8) | 250;
        pix_colore[MAGENTA_] = (250<<16) | (0<<8) | 250;
        pix_colore[MARRON_] = (150<<16) | (100<<8) | 50;

        gcv.foreground = pix_colore[c];
        gc = XCreateGC(dpy, root, GCForeground, &gcv);
        *depth = DefaultDepth(dpy, scr);

        if (*depth<=8) {
            fprintf(stderr, "Can't use X Window output in depth <= 8. Aborting!!\n");
            retour.w=0;
            return(retour);
        }

        win = XCreateSimpleWindow(dpy, root, 10, 10, width, height, 0, pix_colore[BLANC]/*white*/, pix_colore[c]);
        retour.pixmap = XCreatePixmap(dpy, win, width, height, *depth);
        for (j=0; j<height; j++)
            for (i=0; i<width; i++)
                XDrawPoint(dpy, retour.pixmap, gc, i, j);

        if (*depth == 16 && visual->green_mask==992) *depth = 15;
        wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
        wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(dpy, win, &wm_delete_window, 1);
        xfont = XLoadQueryFont(dpy, "9x15");
        XSelectInput(dpy, win, KeyPressMask|VisibilityChangeMask|ExposureMask|PointerMotionMask| StructureNotifyMask|ButtonPressMask|ButtonReleaseMask);

        XMapWindow(dpy, win);
        XFlush(dpy);
    } // if dpy==NULL

    if (dpy!=NULL) {
        XResizeWindow(dpy, win, width, height);
        XSync(dpy, True);
    }

    retour.dpy=dpy;
    retour.win=win;
    retour.gc=gc;
    retour.w=width;
    retour.h=height;
    retour.visual=visual;

    return(retour);
}
/* fin de la fonction cree_fenetre_coloree */


window_t create_window(int width, int height, int *depth){
    return(create_window_with_background_color(width, height, depth, NOIR));
}



/* Met à jour l'affichage après des tracés dans le pixmap */
void actualise_fenetre(window_t fen){
    XSetWindowBackgroundPixmap(dpy, fen.win, fen.pixmap);
    XClearWindow(dpy, fen.win);
}


/* gère les évènements clavier et souris */
int manage_event(window_t fen, XEvent event)
{
    if ((event.xclient.message_type == wm_protocols &&
        event.xclient.format == 32 &&
        event.xclient.data.l[0] == wm_delete_window))
    return(2);

    if (event.type == VisibilityNotify || event.type == Expose) {
        actualise_fenetre(fen);
        XSync(fen.dpy, True);
        return(1);
    }

    if (event.type == KeyPress) {
        KeySym keysym;
        char buffer[1];
        XLookupString((XKeyEvent *) &event, buffer, 1, &keysym, NULL);
        if (keysym=='h' || keysym=='H') {
                //display_bindings();
            //bindings = 1;
            return(1);
        }
        if (keysym=='q' || keysym=='Q') return(2);

    } // fin de if (event.type == KeyPress)

    return(0);
}
/* end of function "manage_event" */

/* fonction do_output_pixel */
void do_output_pixel(window_t f, int u, int v, Pixel p)
{
    if((u>=0)&&(u<f.w)&&(v>=0)&&(v<f.h)){
        XSetForeground(f.dpy, f.gc, p);
        XDrawPoint(f.dpy, f.pixmap, f.gc, u, v);
    }
}
/* fin de la fonction do_output_pixel */


/* fonction do_output */
void do_output(window_t f, int u, int v, couleur n)
{
    do_output_pixel(f, u, v, pix_colore[n]);
}
/* fin de la fonction do_output */


/* écrit du texte dans la fenêtre principale */
void display_text(window_t f, int x, int y, char * text, couleur c){
    char line[80]; // chaine à afficher
    char *ptr1, *ptr2; // sert au découpage de la chaine en lignes
    int i, n; // i:numero de ligne, n: nb de caractères à afficher sur la ligne i (avant le saut de ligne)

    XSetFont(f.dpy, f.gc, xfont->fid);
    XSetForeground(f.dpy, f.gc, pix_colore[c]);

    i = 0;

    ptr1 = text;
    while (ptr1) {
        ptr2 = index(ptr1, '\n');
        if (!ptr2)
            strcpy(line, ptr1);
        else {
            n = ptr2 - ptr1;
            strncpy(line, ptr1, n);
            line[n] = '\0';
        }

        XDrawString(f.dpy, f.pixmap, f.gc, x, y+24*i, line, strlen(line));
        ++i;

        if (ptr2)
            ptr1 = ptr2+1;
        else
            ptr1 = NULL;

    /* affichage du résultat dans la fenetre */
    actualise_fenetre(f);
    }
}
/* end of function display_text */


void display_text_blanc(window_t f, int x, int y, char * text){
    display_text(f,x,y,text, BLANC);

}


/*trace un point en coordonnées fenêtre */
int trace_point(window_t f, int x, int y, couleur c){
    do_output(f, x, y, c);
    /* affichage du résultat dans la fenetre */
    /*actualise_fenetre(f); // too long is done for each point */
    return(0);
}

/* close displays */
int close_window(window_t fen){
    XDestroyWindow(fen.dpy,fen.win);
    XFreePixmap(dpy, fen.pixmap);
    XFreeGC(fen.dpy, fen.gc);
    XFreeFont(dpy, xfont);

    /* fermeture du display général */
    if (dpy!=NULL) {
        XCloseDisplay(dpy);
    }
    return(0);
}


/**
 * Draw lines on a window.
 * 
 * \param f window for drawing
 * \param points points array in window's coordinates
 * \param nb number of points
 * \param n lines' color
 * \param largeur lines' width 
 */
void trace_lignes(window_t f, XPoint *points, int nb, couleur n, int largeur){
    XSetForeground(f.dpy, f.gc, pix_colore[n]);
    XSetLineAttributes(f.dpy, f.gc, largeur, LineSolid, CapRound, JoinMiter);
    XDrawLines(f.dpy, f.pixmap, f.gc, points, nb, CoordModeOrigin);
    actualise_fenetre(f);
}


/* remplissage d'un polygone */
void remplit(window_t f, XPoint *points, int nb, couleur n){
/* f : fenetre du tracé
** points : la liste des points decrivant le contour
** nb : le nb de points
** n : couleur de l'interieur */

    XSetForeground(f.dpy, f.gc, pix_colore[n]);
    XFillPolygon(f.dpy, f.pixmap, f.gc, points, nb, Complex, CoordModeOrigin);
    actualise_fenetre(f);
}


/* enregistre le dessin d'une fenetre dans un fichier */
void sauve_fenetre_jpeg(window_t *f, char *filename){
    yImage *im; /* pour sortie dans un fichier */

    im=create_yImage_from_drawable(f->win, f->visual, f->dpy, 0, 0, f->w, f->h);
    y_save_jpeg(im, filename);
    y_destroy_image(im);
}

/* enregistre le dessin d'une fenetre dans un fichier */
void sauve_fenetre_png(window_t *f, char *filename){
    yImage *im; /* pour sortie dans un fichier */

    im=create_yImage_from_drawable(f->win, f->visual, f->dpy, 0, 0, f->w, f->h);
    y_save_png(im, filename);
    y_destroy_image(im);
}

/* enregistre le dessin d'une fenetre dans un fichier */
void sauve_fenetre_tiff(window_t *f, char *filename){
    yImage *im; /* pour sortie dans un fichier */

    im=create_yImage_from_drawable(f->win, f->visual, f->dpy, 0, 0, f->w, f->h);
    y_save_tiff(im, filename);
    y_destroy_image(im);
}

