/**
 * \file coordonnees.c
 * \brief Extensive Lambert II Geographical Coordinates Transformations
 * \since 11/04/2007
 */

#include "coordonnees.h"

#include <math.h>


/* constantes de projection Lambert II étendu */
#define C_N 0.7289686274
#define C_C 11745793.39
#define C_XS 600000
#define C_YS 8199695.768

/* excentricite de l'ellipsoide */
#define C_E 0.08248325676
/* longitude du méridien central : 2°20'14,025'' E de Greenwich */
# define C_L0 2.3372291667


/* constantes de projection Lambert 93 -> géoide différent (RGF93)*/
#define C93_N 0.7256077650
#define C93_C 11754255.426
#define C93_XS 700000
#define C93_YS 12655612.05
/* uses 3°E from Greenwich meridian */
#define C93_L0 3
// TODO must find the real excentricity
#define C93_E C_E



/** Converts degrees to radians */
static float radians(float degres){ return (degres*M_PI/180); }


/**
 * Calculate Lambert coordinate from lat/lon of the same geodesic
 * system.
 */
static coord_lamb calculate_plane_coordinates(coord_geo coord, int c_e, int c_c, int c_n, int c_l0, int c_xs, int c_ys) {
    coord_lamb retour; /* la valeur de retour */

    /* calculs intermédiaires */
    float S=sinf(radians(coord.phi));
    float L=0.5*logf( (1+S)/(1-S) ) - 0.5*e*logf( (1+e*S)/(1-e*S) );
    float R=c*expf(-n*L);
    float gamma=n*(coord.lambda-l0);

    /* Calcul du resultat */
    retour.X=xs+R*sinf(radians(gamma));
    retour.Y=ys-R*cosf(radians(gamma));

    return(retour);
}


/**
 * Lambert II coordinates calculation from lat/lon of the NTF system
 * (Clarke's geoide).
 */
coord_lamb calcule_Lambert(coord_geo coord) {
    return calculate_plane_coordinates(coord, C_E, C_C, C_N, C_L0, C_XS, C_YS);
}


/**
 * Lambert 93 coordinates calculation from lat/lon of the RGF93 system.
 */
coord_lamb calcule_Lambert93(coord_geo coord) {
    return calculate_plane_coordinates(coord, C93_E, C93_C, C93_N, C93_L0, C93_XS, C93_YS);
}



coord_lamb correction_WGS84(coord_lamb brut){
    coord_lamb corrige; // valeur de retour

    corrige.X=brut.X+5000;
    corrige.Y=brut.Y-7000;

    return(corrige);
}
