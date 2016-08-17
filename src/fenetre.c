/* fenetre.c */

#include "fenetre.h"

/* sortie vers un fichier */
#include "yXinterface.h"
#include "yImage.h"

/* variables du XSystem */
Display * dpy;
Pixmap pixmap;
GC gc;

XGCValues gcv;

Atom wm_delete_window, wm_protocols;
XFontStruct *xfont;

Window root, win;

unsigned char *red, *green, *blue;




/* Initialisation des variables globales */
void init_Xvariable(){
	red=NULL; green=NULL; blue=NULL;
	dpy=NULL;
	pix=NULL;
}



/* Initialisation de la palette de couleurs */
/* la palette "range" est passée en paramètre */
/* initialisation des tableaux de 24000 valeurs : red, green, blue ainsi que pix(le tableau de Pixels XWindows)*/
void init_colors(char *range, int depth, int gamma_f, int hmax)
{
	int i, j, k, boo; // j:valeur de niveau, i:valeur du précédent niveau, k:valeur intermédaire pour interpolation,  boo:couleur associée au niveau j trouvée
	char *ptr1, *ptr2; // chaines extraites de "range"
	unsigned int col; // couleur correspondant au niveau "j"
	unsigned char c1, c2, c3; // couleurs RGB du précendent niveau
	unsigned char u, v; // utilisés pour calculer la couleur du pixel lorsque "depth" != 24
	double f; // facteur pour l'interpolation
	int verbose=0;

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
		if (depth>=24)
			pix[k] = (red[k]<<16) | (green[k]<<8) | blue[k];
		else
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
}
/* fin de init_colors */



/* retourne w=0 si echec */
fenetre cree_fenetre_coloree(int width, int height, int *depth, float x1, float x2, float y1, float y2, couleur c){
	int i, j; //compteurs
	fenetre retour;
	int scr;
	Visual * visual;

	if (dpy==NULL) {
		dpy = XOpenDisplay(NULL);
		if(dpy==NULL) {
			fprintf(stderr, "Ouverture du display impossible");
			exit(1);
		}
		scr = DefaultScreen(dpy);
		visual = DefaultVisual(dpy, scr);
		root = DefaultRootWindow(dpy);
		//black = BlackPixel(dpy, scr);
		//white = WhitePixel(dpy, scr);
		
		pix_colore[NOIR] = BlackPixel(dpy, scr);
		pix_colore[BLANC] = WhitePixel(dpy, scr);
		pix_colore[ROUGE] = (250<<16) | (0<<8) | 0;
		pix_colore[VERT] = (0<<16) | (250<<8) | 0;
		pix_colore[BLEU] = (0<<16) | (0<<8) | 250;
		pix_colore[ORANGE] = (250<<16) | (150<<8) | 0;
		pix_colore[JAUNE] = (250<<16) | (250<<8) | 0;
		pix_colore[CYAN] = (0<<16) | (250<<8) | 250;
		pix_colore[MAGENTA] = (250<<16) | (0<<8) | 250;
		pix_colore[MARRON] = (150<<16) | (100<<8) | 50;

		gcv.foreground = pix_colore[c]/*black*/;
		gc = XCreateGC(dpy, root, GCForeground, &gcv);
		*depth = DefaultDepth(dpy, scr); //depth = DefaultDepth(dpy, scr);
 
		if (*depth<=8) {
			fprintf(stderr, "Can't use X Window output in depth <= 8. Aborting!!\n");
			retour.w=0;
			return(retour);
		}
		/*else
			cmap = DefaultColormap(dpy, scr);*/ 

		win = XCreateSimpleWindow(dpy, root, 10, 10, width, height, 0, pix_colore[BLANC]/*white*/, pix_colore[c]);
		pixmap = XCreatePixmap(dpy, win, width, height, *depth);
		for (j=0; j<height; j++)
			for (i=0; i<width; i++)
				XDrawPoint(dpy, pixmap, gc, i, j);
		
		if (*depth == 16 && visual->green_mask==992) *depth = 15;
		wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
		wm_delete_window = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(dpy, win, &wm_delete_window, 1);
		xfont = XLoadQueryFont(dpy, "9x15");
		XSelectInput(dpy, win, KeyPressMask|VisibilityChangeMask|ExposureMask|PointerMotionMask| StructureNotifyMask|ButtonPressMask|ButtonReleaseMask);

		XMapWindow(dpy, win);
		XFlush(dpy);
		//usleep(10000);
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
	retour.x1=x1; retour.x2=x2; retour.y1=y1; retour.y2=y2;
	retour.visual=visual;

	return(retour);
}
/* fin de la fonction cree_fenetre_coloree */

/* retourne w=0 si echec */
fenetre cree_fenetre(int width, int height, int *depth, float x1, float x2, float y1, float y2){
	return(cree_fenetre_coloree(width, height, depth, x1, x2, y1, y2, NOIR));
}
/* fin de la fonction cree_fenetre */




/* Met à jour l'affichage après des tracés dans le pixmap */
void actualise_fenetre(fenetre fen){
	XSetWindowBackgroundPixmap(dpy, fen.win, pixmap);
	XClearWindow(dpy, fen.win);
}


/* gère les évènements clavier et souris */
int manage_event(fenetre fen, XEvent event)
{
	if ((event.xclient.message_type == wm_protocols &&
		event.xclient.format == 32 &&
		event.xclient.data.l[0] == wm_delete_window))
	return(2);

	if (event.type == VisibilityNotify || event.type == Expose) {
		//XSetWindowBackgroundPixmap(dpy, fen.win, pixmap);
		//XClearWindow(dpy, fen.win); //on remet le pixmap dans le fond de la fenetre
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
/* fin de la fonction "manage_event" */

/* fonction do_output_pixel */
void do_output_pixel(fenetre f, int u, int v, Pixel p)
{
	if((u>=0)&&(u<f.w)&&(v>=0)&&(v<f.h)){
		XSetForeground(f.dpy, f.gc, p);
		XDrawPoint(f.dpy, /*f.win*/pixmap, f.gc, u, v); 
		//XDrawPoint(dpy, pixmap, gc, x, y); 
	}
}
/* fin de la fonction do_output_pixel */


/* fonction do_output */
void do_output(fenetre f, int u, int v, couleur n)
{
	do_output_pixel(f, u, v, pix_colore[n]);
}
/* fin de la fonction do_output */

/* fonction trace_ligne */
/* (utilise les coordonnées des points en Lambert) */
void trace_ligne(fenetre f, float u1, float u2, float v1, float v2, couleur n, int largeur){
	int x,y,z,t; /* coordonnees fenetres */
	
	/* transformations du GC */
	XSetForeground(f.dpy, f.gc, pix_colore[n]);
	XSetLineAttributes(f.dpy, f.gc, largeur, LineSolid, CapRound, JoinMiter);	

	/* calcul des coordonées fenetre */
	/*x=transforme_xLambert(u1, f.w, f.x1, f.x2);
	y=transforme_yLambert(v1, f.h, f.y1, f.y2);
	z=transforme_xLambert(u2, f.w, f.x1, f.x2);
	t=transforme_yLambert(v2, f.h, f.y1, f.y2);*/
	x=transforme_x(u1, f.w, f.x1, f.x2);
	y=transforme_y(v1, f.h, f.y1, f.y2);
	z=transforme_x(u2, f.w, f.x1, f.x2);
	t=transforme_y(v2, f.h, f.y1, f.y2);
	
	

	/* tracé de la ligne */
	XDrawLine(f.dpy, /*f.win*/pixmap, f.gc, x, y, z, t);

	/* affichage du résultat dans la fenetre */
	/*actualise_fenetre(f);*/
}
/* fin de la fonction trace_ligne */


/* écrit du texte dans la fenêtre principale */
void display_text(fenetre f, int x, int y, char * text, couleur c){
	char line[80]; // chaine à afficher
	char *ptr1, *ptr2; // sert au découpage de la chaine en lignes
	int i, n; // i:numero de ligne, n: nb de caractères à afficher sur la ligne i (avant le saut de ligne)

	XSetFont(f.dpy, f.gc, xfont->fid);
	XSetForeground(f.dpy, f.gc, /*white*/ pix_colore[c]);

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

		XDrawString(f.dpy, /*f.win*/pixmap, f.gc, x/*+6*/, y+24*i, line, strlen(line));
		++i;

		if (ptr2) 
			ptr1 = ptr2+1;
		else
			ptr1 = NULL;

	/* affichage du résultat dans la fenetre */
	actualise_fenetre(f);
	}
}
void display_text_blanc(fenetre f, int x, int y, char * text){
	display_text(f,x,y,text, BLANC);

}
/* fin de la fonction display_text */





/*trace un point en coordonnées fenêtre */
int trace_point(fenetre f, int x, int y, couleur c){
	do_output(f, x, y, c);
	/* affichage du résultat dans la fenetre */
	/*actualise_fenetre(f);*/
	return(0);
}


/* trace un point en fct des coordonnées Lambert */
int pointe_pixel(fenetre f, float xLamb, float yLamb, int L, /*couleur c*/Pixel p, forme_t forme) {
	int x, y; // les coordonnées sur la carte
	int k, l; //compteurs
	//int L=1; //demi largeur du point

	/* passage des coordonnées cartes aux coordonnées fenêtre */
	x=transforme_x(xLamb, f.w, f.x1, f.x2)-1; /* les coord fenetre vont de 0 à w-1 */
	y=transforme_y(yLamb, f.h, f.y1, f.y2)-1;
	//printf("%f %d %f %f %d\n", xLamb, f.w, f.x1, f.x2, x);
	
	/*if(x==-2)*/if(x<0) return(1);
	/*if(y==-2)*/if(y<0) return(1);
	if((x>f.w)||(y>f.h)) return(2);

	if(L==0) forme=CARRE;
	//printf("%d %d : ",x,y);
	if (forme==CARRE) // carré
		for(k=-L; k<=L; k++)
			for(l=-L; l<=L; l++)
				do_output_pixel(f,x+l,y+k, p);
	else if(forme==TRIANGLE) // triangle
		for(k=-L; k<=L+1; k++)
			for(l=-(k+L)*2/3; l<=(k+L)*2/3; l++)
				do_output_pixel(f,x+l,y+k, p);
	else if(forme==ROND) //rond
		for(k=-L; k<=L; k++)
			for(l=-floor(sqrt(L*L*10000-10000*k*k)/100); l<floor(sqrt(10000*L*L-10000*k*k)/100); l++)
				do_output_pixel(f,x+l,y+k, p);

	/* affichage du résultat dans la fenetre */
	//actualise_fenetre(f); //trop lent si on le fait pour chaque point
	return(0);
}
int pointe(fenetre f, float xLamb, float yLamb, int L, couleur c, forme_t forme) {
	return(pointe_pixel(f, xLamb, yLamb, L, pix_colore[c], forme));
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

	if(fen.w<300) return; //trop petite fenetre
	if(fen.h<100) return;
	largeur = fen.w/10; // taille de base de l'échelle

	terrain=(fen.x2-fen.x1)*largeur/fen.w; // en hectomètres

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
	largeur=terrain*fen.w/(fen.x2-fen.x1);

	/* calcul de l'étiquette */
	if (terrain >= 40) {
		terrain=terrain/10;
		sprintf(unite,"km");
	} else {
		terrain=terrain*100;
		sprintf(unite,"m");
	}
	sprintf(etiquette, "%d %s", terrain, unite);

	u=fen.w-largeur*12/10;
	v=fen.h-15;

	XSetForeground(fen.dpy, fen.gc, pix_colore[cdessin]);
	XSetLineAttributes(fen.dpy, fen.gc, 0, LineSolid, CapRound, JoinMiter);
	XDrawLine(fen.dpy, pixmap/*fen.win*/, fen.gc, u, v, u+largeur, v);
	XDrawLine(fen.dpy, pixmap/*fen.win*/, fen.gc, u, v+1, u+largeur, v+1);
	XDrawLine(fen.dpy, pixmap/*fen.win*/, fen.gc, u, v+2, u+largeur, v+2);
	XDrawLine(fen.dpy, pixmap/*fen.win*/, fen.gc, u, v-1, u, v-4);
	XDrawLine(fen.dpy, pixmap/*fen.win*/, fen.gc, u+1, v-1, u+1, v-4);
	XDrawLine(fen.dpy, pixmap/*fen.win*/, fen.gc, u+largeur, v-1, u+largeur, v-4);
	XDrawLine(fen.dpy, pixmap/*fen.win*/, fen.gc, u+largeur-1, v-1, u+largeur-1, v-4);
	display_text(fen, u+(largeur-10*strlen(etiquette))/2, v-7, etiquette, ctexte);

	/* affichage du résultat dans la fenetre */
	actualise_fenetre(fen);
}
/* fin de la fonction trace_echelle */

/* fermeture des display */
int fermeture(fenetre fen){
	/* fermeture de la fenetre */
	XDestroyWindow(fen.dpy,fen.win);
	XFreePixmap(dpy, pixmap);
	XFreeGC(fen.dpy, fen.gc);
	XFreeFont(dpy, xfont);

	/* free pix and color buffers */
	if(pix) free(pix);
	free(red); free(green); free(blue);

	/* fermeture du display général */
	if (dpy!=NULL) {
		XCloseDisplay(dpy);
	}
	return(0);
}


/* fctionne en coordonnées fenetres */
void trace_lignes(fenetre f, XPoint *points, int nb, couleur n, int largeur){
/* f : fenetre du tracé
** points : la liste des points à relier
** nb : le nb de points
** n : couleur du tracé
** largeur : épaisseur du trait */

	int err; /* détection d'une erreur */

	XSetForeground(f.dpy, f.gc, pix_colore[n]);
	XSetLineAttributes(f.dpy, f.gc, largeur, LineSolid, CapRound, JoinMiter);
	err=XDrawLines(f.dpy, pixmap, f.gc, points, nb, CoordModeOrigin);
	actualise_fenetre(f);
	
	//if(err!=0) printf("trace_ligne: %d", err);
}


/* remplissage d'un polygone */
void remplit(fenetre f, XPoint *points, int nb, couleur n){
/* f : fenetre du tracé
** points : la liste des points decrivant le contour
** nb : le nb de points
** n : couleur de l'interieur */

	XSetForeground(f.dpy, f.gc, pix_colore[n]);
	XFillPolygon(f.dpy, pixmap, f.gc, points, nb, Complex, CoordModeOrigin);
	actualise_fenetre(f);
}


/* enregistre le dessin d'une fenetre dans un fichier */
void sauve_fenetre_jpeg(fenetre *f, char *filename){
	yImage *im; /* pour sortie dans un fichier */

	im=create_yImage_from_drawable(f->win, f->visual, f->dpy, 0, 0, f->w, f->h);
	sauve_jpeg(im, filename);
	destroy_yImage(im);
}

/* enregistre le dessin d'une fenetre dans un fichier */
void sauve_fenetre_png(fenetre *f, char *filename){
	yImage *im; /* pour sortie dans un fichier */

	im=create_yImage_from_drawable(f->win, f->visual, f->dpy, 0, 0, f->w, f->h);
	sauve_png(im, filename);
	destroy_yImage(im);
}

/* enregistre le dessin d'une fenetre dans un fichier */
void sauve_fenetre_tiff(fenetre *f, char *filename){
	yImage *im; /* pour sortie dans un fichier */

	im=create_yImage_from_drawable(f->win, f->visual, f->dpy, 0, 0, f->w, f->h);
	sauve_tiff(im, filename);
	destroy_yImage(im);
}

