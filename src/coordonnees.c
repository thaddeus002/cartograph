/* coordonnees.c
** transformations de coordonnées geographiques en Lambert
** 11/04/2007
*/

#include "coordonnees.h"

/* constantes de projection Lambert 93 */
/*#define C_N 0.7256077650
#define C_C 11754255.426
#define C_XS 700000
#define C_YS 12655612.05*/

/* constantes de projection Lambert II étendu */
#define C_N 0.7289686274
#define C_C 11745793.39
#define C_XS 600000
#define C_YS 8199695.768


/* excentricite de l'éllipsoide */
#define C_E 0.08248325676
/* longitude du méridien central : 2°20'14,025'' E de Greenwich */
# define C_L0 2.3372291667

/* transforme des degrés en radians */
float radians(float degres){ return (degres*M_PI/180); }


/* le calcul des coordonnées Lambert */
coord_lamb calcule_Lambert(coord_geo coord) {
	coord_lamb retour; /* la valeur de retour */
	
	/* calculs intermédiaires */
	float S=sinf(radians(coord.phi));
	float L=0.5*logf( (1+S)/(1-S) ) - 0.5*C_E*logf( (1+C_E*S)/(1-C_E*S) );
	float R=C_C*expf(-C_N*L);
	float gamma=C_N*(coord.lambda-C_L0);

	/* Calcul du resultat */
	retour.X=C_XS+R*sinf(radians(gamma));
	retour.Y=C_YS-R*cosf(radians(gamma));
	
	return(retour);	
}


/* correction de passage entre les systèmes internationnal et francais */
coord_lamb correction_WGS84(coord_lamb brut){
	coord_lamb corrige; // valeur de retour
	
	corrige.X=brut.X+5000;
	corrige.Y=brut.Y-7000;
	
	return(corrige);
}
