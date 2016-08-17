/* programme de test de la bibliothèque de lecture des shapefiles */

//#include <stdio.h>
//#include <stdlib.h>

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
	XPoint *points; /* la forme */
	int N; /*nb de points*/
	int Np; /* nb de parties */
	int i; /* compteur */

	if(sh==NULL) return(1);

	if((sh->shapeType==POLYGON) || (sh->shapeType==POLYLINE)){
		N=sh->numPoints;
		points=malloc(N*sizeof(XPoint));
		if(points==NULL){
			//fprintf(stderr, "Allocation mémoire impossible\n");
			return(2);
		}

		for(i=0; i<=N-1; i++){
			points[i].x=transforme_x(sh->points[i].x, f.w, f.x1, f.x2);
			points[i].y=transforme_y(sh->points[i].y, f.h, f.y1, f.y2);
		}


		Np=sh->numParts;
		for(i=0; i<=Np-2; i++){
			if(/*(*/sh->shapeType==POLYGON/*)&&(points[sh->parts[i]].x==points[sh->parts[i+1]].x)&&(points[sh->parts[i]].y==points[sh->parts[i+1]].y)*/)
				remplit(f,points+sh->parts[i],sh->parts[i+1]-sh->parts[i],ROUGE);
			trace_lignes(f,points+sh->parts[i],sh->parts[i+1]-sh->parts[i],JAUNE,1);
		}
		if(/*(*/sh->shapeType==POLYGON/*)&&(points[sh->parts[Np-1]].x==points[N-1].x)&&(points[Np-1].y==points[N-1].y)*/)
			remplit(f, points+sh->parts[Np-1],N-sh->parts[Np-1],ROUGE);
		trace_lignes(f, points+sh->parts[Np-1], N-sh->parts[Np-1],JAUNE, 1);


		
	
		
		free(points);
	} /*if(sh->shapeType==POLYGON)*/
	else {
		fprintf(stderr, "Forme non prise en compte : %d\n", sh->shapeType);
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
		XNextEvent(f.dpy, &event);
		i = manage_event(f, event);
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

