/* auteur : Yannick */
/* date : 12 septembre 2008 */


#include "shape_types.h"




GenericShape *init_shape(){
	GenericShape *sh;
	sh=malloc(sizeof(GenericShape));
	
	sh->shapeType=0;
	
	sh->numParts=0;
	sh->numPoints=0;
	sh->parts=NULL;
	sh->partTypes=NULL;
	sh->points=NULL;
	sh->zArray=NULL;
	sh->mArray=NULL;
	
	return(sh);	
}

int destroy_shape(GenericShape *sh){
	if(sh->parts!=NULL) free(sh->parts);
	if(sh->partTypes!=NULL) free(sh->partTypes);
	if(sh->points!=NULL) free(sh->points);
	if(sh->zArray!=NULL) free(sh->zArray);
	if(sh->mArray!=NULL) free(sh->mArray);
	
	free(sh);
	return(0);
}
