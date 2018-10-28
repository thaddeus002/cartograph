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



/**********************************************************//**
 *    GEOGRAPHICAL (Lambda,Phy,h) TO PLANE (LAMBERT)          *
 *************************************************************/


/** Converts degrees to radians */
static float radians(float degres){ return (degres*M_PI/180); }


/**
 * Calculate Lambert coordinate from lat/lon of the same geodesic
 * system.
 */
static coord_lamb calculate_plane_coordinates(coord_geo coord, double e, double c, double n, double l0, double xs, double ys) {
    coord_lamb retour; /* return value */

    /* intermediary calculations */
    double S=sinf(radians(coord.phi));
    double L=0.5*logf( (1+S)/(1-S) ) - 0.5*e*logf( (1+e*S)/(1-e*S) );
    double R=c*expf(-n*L);
    double gamma=n*(coord.lambda-l0);

    /* result */
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


/**********************************************************//**
 *    3D CARTESIAN (X,Y,Z) TO GEOGRAPHICAL (Lambda,Phy,h)     *
 *************************************************************/

/**
 * Calculate the excentricity of an ellipsoide.
 * TODO a and b may be more precise and have decimals
 */
static double e_d(int a, int b) {
    return sqrt(((float)(a*a-b*b))/a*a);
}

static double f_d(double e) {
    return 1 - sqrt(1-e*e);
}

/**
 * Calculate the distance of a point from the coordinates center.
 */
static double r_d(coord_cartesian coord) {
    return sqrt(coord.X*coord.X+coord.Y*coord.Y+coord.Z*coord.Z);
}


static double mu_d(coord_cartesian coord, int a, double e) {

    double k = coord.Z / sqrt(coord.X*coord.X + coord.Y*coord.Y);
    double alpha = (1 - f_d(e))+(e*e*a/r_d(coord));
    return atan(k*alpha);
}

coord_geo cartesian2geo(coord_cartesian coord, int a, int b) {

    coord_geo value;

    value.lambda = atan2f(coord.Y, coord.X);

    double e = e_d(a, b);
    double f = f_d(e);
    double mu = mu_d(coord, a, b);
    double smu = sin(mu);
    double cmu = cos(mu);

    double N = coord.Z * (1-f) + e*e*a*smu*smu*smu;
    double D = (1-f)*(sqrt(coord.X*coord.X+coord.Y*coord.Y)-e*e*a*cmu*cmu*cmu);

    value.phi = atan(N/D);

    double cphi = cos(value.phi);
    double sphi = sin(value.phi);

    value.h = (sqrt(coord.X*coord.X+coord.Y*coord.Y)*cphi) + (coord.Z*sphi) - (a*sqrt(1-e*e*sphi*sphi));

    return value;
}


/**********************************************************//**
 *    GEOGRAPHICAL (Lambda,Phy,h) TO 3D CARTESIAN (X,Y,Z)     *
 *************************************************************/


coord_cartesian geo2cartesian(coord_geo coord, int a, int b) {

    coord_cartesian cartesian;

    double e = e_d(a, b);
    double sphi = sin(coord.phi);

    double W = sqrt(1-e*e*sphi*sphi);
    double N = a / W;

    cartesian.X = (N+coord.h) * cos(coord.phi) * cos(coord.lambda);
    cartesian.Y = (N+coord.h) * cos(coord.phi) * sin(coord.lambda);
    cartesian.Z = (N*(1-e*e)+coord.h) * sphi;

    return cartesian;
}
