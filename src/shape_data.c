/* lecture de données dans un fichier shp */
/* auteur : Yannick */
/* date : 26 juin 2008 */


#include "shape_data.h"
#include <string.h>



/** Lecture de l'entete **/
/* renvoie un pointeur sur une entete */
/* et affiche des infos à l'écran - A REVOIR */
shape_header *lit_entete(FILE *fd){
	shape_header *sh;
	unsigned char big[4]; /* entiers lus à big endien à transformer en little */
		
	sh=malloc(sizeof(shape_header));
	
	
	/* type de fichier */
	fread(big, sizeof(char), 4, fd);
	sh->fileCode=big[0]<<24 | big[1] << 16 | big[2] << 8 | big[3];
	if(sh->fileCode!=9994) {
		printf("Type de fichier incorrect : %d\n", sh->fileCode);
		return(NULL);
	}
	
	
	/*5 entiers non utilises*/
	fseek(fd, 5*sizeof(int), SEEK_CUR);
	
	/*file lenght*/
	fread(big, sizeof(char), 4, fd);
	sh->lenght=big[0]<<24 | big[1] << 16 | big[2] << 8 | big[3];
	/*taille=sh->lenght-50; /* 50 * 16 bits = 100 octets de l'entete */


	/*Version*/
	fread(&sh->version, sizeof(int), 1, fd);


	/*shape type*/
	fread(&sh->shapeType, sizeof(int), 1, fd);
	switch(sh->shapeType){
		case 0:
		case 1:
		case 3:
		case 5:
		case 8:
		case 11:
		case 13:
		case 15:
		case 18:
		case 21:
		case 23:
		case 25:
		case 28:
		case 31: break;
		default : return(NULL);
	}
	
	
	/*les bordures */
	/*Xmin Ymin Xmax Ymax Zmin Zmax Mmin Mmax*/
	fread(sh->boundarieBox, sizeof(double), 8, fd);


	/* Fin de l'entête */	
	return(sh);
}
/** Fin de la lecture de l'entete **/


/* Décripte les infos de l'entete */
int affiche_entete(shape_header *sh){
	char *forme; /* le type de formes */

	if(sh==NULL){
		printf("Entete non valide\n");
		return(1);
	}

	if(sh->fileCode!=9994) {
		printf("Type de fichier incorrect : %d\n", sh->fileCode);
		return(2);
	} else {
		printf("Fichier SHP\n");
	}
	printf("Longueur du fichier : %d octets\n", sh->lenght*2);
	printf("Numéro de version : %d\n", sh->version);

	switch(sh->shapeType){
		case 0:  forme="Null Shape"; break;
		case 1:  forme="Point"; break;
		case 3:  forme="PolyLine"; break;
		case 5:  forme="Polygon"; break;
		case 8:  forme="MultiPoint"; break;
		case 11: forme="PointZ"; break;
		case 13: forme="PolyLineZ"; break;
		case 15: forme="PolygonZ"; break;
		case 18: forme="MultiPointZ"; break;
		case 21: forme="PointM"; break;
		case 23: forme="PolyLineM"; break;
		case 25: forme="PolygonM"; break;
		case 28: forme="MultiPointM"; break;
		case 31: forme="MultiPatch"; break;
		default : forme="Inconnu";
	}
	printf("type de forme : %d - %s\n", sh->shapeType, forme);
	if(!strcmp(forme, "Inconnu")){
		return(3);
	}

	printf("Xmin - Xmax : %8f - %8f \n", sh->boundarieBox[0], sh->boundarieBox[2]);
	printf("Ymin - Ymax : %8f - %8f \n", sh->boundarieBox[1], sh->boundarieBox[3]);
	printf("Zmin - Zmax : %8f - %8f \n", sh->boundarieBox[4], sh->boundarieBox[5]);
	printf("Mmin - Mmax : %8f - %8f \n", sh->boundarieBox[6], sh->boundarieBox[7]);
	printf("\n\n");
	
	return(0);
}



int detruit_entete(shape_header *sh){
	free(sh);
	return(0);
}






int affiche_infos(GenericShape *p){
	int i; /* compteur */

	printf("Nb de points : %d\n", p->numPoints);
	printf("Nb de parties : %d\n", p->numParts);
	for(i=1; i<=p->numParts; i++){
		printf("%4d - ", p->parts[i-1]);
	}
	printf("\n");
	/*for(i=1; i<=p->numPoints; i++){
		printf("%6f - %6f\n", p->points[i-1].x, p->points[i-1].y);
	}*/
	
	return(0);
}



/* Enregistrements generiques */
/*Enregistrement *lit_enregistrement(FILE *fd){

	Enregistrement *p;
	p=malloc(sizeof(Polygon));
	
	fread(p->box, sizeof(double), 4, fd);
	fread(&(p->numParts), sizeof(int), 1, fd);
	fread(&(p->numPoints), sizeof(int), 1, fd);
	p->parts=malloc(sizeof(int)*p->numParts);
	fread(p->parts, sizeof(int), p->numParts, fd);
	p->points=malloc(sizeof(Point)*p->numPoints);
	fread(p->points, sizeof(Point), p->numPoints, fd);

	return p;
}*/


/*fonction recursive */
int detruit_enregistrement(Enregistrement *e){	
	if(e->sh!=NULL) destroy_shape(e->sh);
	if(e->next!=NULL) detruit_enregistrement(e->next);
	/* si il y a un prev, c'est lui qui a appellé cette fonction */	
	free(e);
	return(0);
}





/* renvoie une liste chainée */
/* à executer apres la lecture de l'entete, avec fd positionné sur le debut des données */
Enregistrement *lecture_fichier(FILE *fd, shape_header *ent){
	Enregistrement *e; /* valeur de retour -> liste chainee */
	Enregistrement *dernier; /* le dernier maillon de la chaine */
	Enregistrement *nouveau; /* à rajouter à la fin de la liste */


	int lu; /* valeur lue dans le fichier */
	unsigned char big[4]; /* entiers lus à big endien à transformer en little */


	int taille; /* taille en nb de groupes de 16 bits de la partie non lue du fichier */
	GenericShape *forme; //Polygon *pg; /* element lu*/
	int num, longueur; /* num et longueur du dernier enregistrement lu */


	/* initialisation */
	e=NULL; dernier=NULL; nouveau=NULL;

	/* Ouverture et lecture du fichier */


	/***** RECUPERATION DE LA TAILLE DU FICHIER ************/
	if(ent==NULL){
		printf("Entete non valide\n");
		return(NULL);
	}	
	taille=ent->lenght-50;/* 50 * 16 bits = 100 octets de l'entete */





	/***** LECTURE DES ENREGISTREMENTS *********/
	/* Début de lecture des enregistrements */
	while(taille>0){


		fread(big, sizeof(char), 4, fd);
		num=big[0]<<24 | big[1] << 16 | big[2] << 8 | big[3];
		printf("Numéro de l'enregistrement : %d\n", num);

		fread(big, sizeof(char), 4, fd);
		longueur=big[0]<<24 | big[1] << 16 | big[2] << 8 | big[3];
		printf("Longueur de l'enregistrement : %d octets\n", longueur*2);

		taille-=4*sizeof(char)+longueur; /* longueur + 2 octets  (en 1/2 octects) */




		forme=init_shape();
		/* le type de forme */
		fread(&lu, sizeof(int), 1, fd);
		//printf("Type de forme : %d\n", lu);

		if(ent->shapeType!=lu){
			printf("Type de forme en désaccord avec l'entete : %d\n", lu);
			printf("lexture interrompue avant la fin\n");
			destroy_shape(forme);
			return(e);
		}

		forme->shapeType=lu;
		
		

		/*refait la même chose de façon plus générale*/
		switch(lu){
			//case 0:  forme="Null Shape"; break;
			case POINT:
				fread(&(forme->x),sizeof(double), 1, fd);
				fread(&(forme->y),sizeof(double), 1, fd);
				break;
			case POINTZ:
				fread(&(forme->x),sizeof(double), 1, fd);
				fread(&(forme->y),sizeof(double), 1, fd);
				fread(&(forme->z),sizeof(double), 1, fd);
				break;
			case POINTM:
				fread(&(forme->x),sizeof(double), 1, fd);
				fread(&(forme->y),sizeof(double), 1, fd);
				fread(&(forme->m),sizeof(double), 1, fd);
				break;


			case MULTIPATCH:

			case POLYLINEZ:
			case POLYGONZ:
			case POLYLINEM:
			case POLYGONM:

			case POLYLINE:
			case POLYGON:
				fread(forme->box,sizeof(double), 4, fd);
				fread(&(forme->numParts), sizeof(int), 1, fd);
				fread(&(forme->numPoints), sizeof(int), 1, fd);
				forme->parts=malloc(sizeof(int)*forme->numParts);
				fread(forme->parts, sizeof(int), forme->numParts, fd);
				if (lu==MULTIPATCH){
					forme->partTypes=malloc(sizeof(int)*forme->numParts);
					fread(forme->partTypes, sizeof(int), forme->numParts, fd);
				}
				forme->points=malloc(sizeof(Point)*forme->numPoints);
				fread(forme->points, sizeof(Point), forme->numPoints, fd);
				break;



			case MULTIPOINTZ:
				//fread(forme->zRange,sizeof(double), 2, fd);
				//fread(forme->y,sizeof(double), 1, fd);
			case MULTIPOINTM:
			case MULTIPOINT:



			default :
				printf("défaut de lecture : %d\n", lu);
				fseek(fd, longueur*2-sizeof(int), SEEK_CUR); /* positionnement sur l'enregistrement suivant */
				destroy_shape(forme);
				forme=NULL;
		}

		// deuxieme partie des traitements
		switch(lu){
			case MULTIPATCH:
			case POLYLINEZ:
			case POLYGONZ:
				fread(forme->zRange,sizeof(double), 2, fd);
				forme->zArray=malloc(sizeof(double)*forme->numPoints);
				fread(forme->zArray, sizeof(double), forme->numPoints, fd);
			case POLYLINEM:
			case POLYGONM:
				fread(forme->mRange,sizeof(double), 2, fd);
				forme->mArray=malloc(sizeof(double)*forme->numPoints);
				fread(forme->mArray, sizeof(double), forme->numPoints, fd);
				break;
		}






		if(forme!=NULL){
			nouveau=malloc(sizeof(Enregistrement));
			nouveau->numero=num;
			nouveau->longueur=longueur;
			nouveau->sh=forme;
			fprintf(stdout,"Enregistrement valide\n");
			affiche_infos(forme);

			/* rajout à la fin de la liste */
			if(dernier==NULL){
				e=nouveau;
			} else {
				dernier->next=nouveau;
				nouveau->prev=dernier;
			}
		
			dernier=nouveau;
		}

	} /* taille == 0 */

	
	return(e);
}
/* FIN DE LA FONCTION lecture_fichier */

