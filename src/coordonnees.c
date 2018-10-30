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

/** Converts radians to degrees */
static float degrees(float radians) { return (radians*180/M_PI); }


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
 * Apply the Extended Lambert II projection of geographical coordinates
 * of the NTF system (Clarke's geoid).
 * \param coord geographical coordinate of a point
 * \return the plane coordinates after the projection
 */
coord_lamb calcule_Lambert(coord_geo coord) {
    return calculate_plane_coordinates(coord, C_E, C_C, C_N, C_L0, C_XS, C_YS);
}


/**
 * Apply the Lambert 93 projection of geographical coordinates of the
 * RGF93 system.
 * \param coord geographical coordinate of a point
 * \return the plane coordinates after the projection
 */
coord_lamb calcule_Lambert93(coord_geo coord) {
    return calculate_plane_coordinates(coord, C93_E, C93_C, C93_N, C93_L0, C93_XS, C93_YS);
}



/**********************************************************//**
 *    3D CARTESIAN (X,Y,Z) TO GEOGRAPHICAL (Lambda,Phy,h)     *
 *************************************************************/

/**
 * Calculate the square excentricity of an ellipsoide.
 * TODO a and b may be more precise and have decimals
 */
static double e2_d(double a, double b) {
    return (a*a-b*b)/(a*a);
}

static double f_d(double e2) {
    return 1 - sqrt(1-e2);
}

/**
 * Calculate the distance of a point from the coordinates center.
 */
static double r_d(coord_cartesian coord) {
    return sqrt(coord.X*coord.X+coord.Y*coord.Y+coord.Z*coord.Z);
}


/**
 * \return an angle in radians
 */
static double mu_d(coord_cartesian coord, double a, double e2) {

    double k = coord.Z / sqrt(coord.X*coord.X + coord.Y*coord.Y);
    double alpha = (1 - f_d(e2))+(e2*a/r_d(coord));
    return atan(k*alpha);
}


coord_geo cartesian2geo(coord_cartesian coord, double a, double b) {

    coord_geo value;

    value.lambda = degrees(atan2f(coord.Y, coord.X));

    double e2 = e2_d(a, b);
    double f = f_d(e2);
    double mu = mu_d(coord, a, e2);
    double smu = sin(mu);
    double cmu = cos(mu);

    double N = coord.Z * (1-f) + e2*a*smu*smu*smu;
    double D = (1-f)*(sqrt(coord.X*coord.X+coord.Y*coord.Y)-e2*a*cmu*cmu*cmu);

    value.phi = degrees(atan(N/D));

    double cphi = cos(radians(value.phi));
    double sphi = sin(radians(value.phi));

    value.h = (sqrt(coord.X*coord.X+coord.Y*coord.Y)*cphi) + (coord.Z*sphi) - (a*sqrt(1-e2*sphi*sphi));

    return value;
}


/**********************************************************//**
 *    GEOGRAPHICAL (Lambda,Phy,h) TO 3D CARTESIAN (X,Y,Z)     *
 *************************************************************/


coord_cartesian geo2cartesian(coord_geo coord, double a, double b) {

    coord_cartesian cartesian;

    double e2 = e2_d(a, b);
    double sphi = sin(radians(coord.phi));
    double W = sqrt(1-e2*sphi*sphi);
    double N = a / W;

    cartesian.X = (N+coord.h) * cos(radians(coord.phi)) * cos(radians(coord.lambda));
    cartesian.Y = (N+coord.h) * cos(radians(coord.phi)) * sin(radians(coord.lambda));
    cartesian.Z = (N*(1-e2)+coord.h) * sphi;

    return cartesian;
}



/**********************************************************//**
 *    GEOIDE CONVERSIONS                                      *
 *************************************************************/

/**
 * Usable for metropolitan France
 */
static coord_cartesian ntf2wgs(coord_cartesian initial) {
    coord_cartesian final;

    final.X = initial.X - 168;
    final.Y = initial.Y - 60;
    final.Z = initial.Z + 320;
    return final;
}


/**
 * Usable for metropolitan France
 */
static coord_cartesian wgs2ntf(coord_cartesian initial) {
    coord_cartesian final;

    final.X = initial.X + 168;
    final.Y = initial.Y + 60;
    final.Z = initial.Z - 320;
    return final;
}


/**********************************************************//**
 *    GEODESIC CONVERSIONS                                    *
 *************************************************************/

// Ellipsoids characteristic
#define NTF_A 6378249.2
#define NTF_B 6356515
#define WGS_A 6378137
#define WGS_B 6356752.314 /*6378137*(1-1/298.257223563)*/


/**
 * Transform WGS84 lat/lon data to Extended Lambert II coordinates.
 */
coord_lamb Wgs84geo_to_Lambert(coord_geo pos) {

    coord_cartesian cartesian_wgs = geo2cartesian(pos, WGS_A, WGS_B);
    coord_cartesian cartesian_ntf = wgs2ntf(cartesian_wgs);
    coord_geo geo_ntf = cartesian2geo(cartesian_ntf, NTF_A, NTF_B);
    return calcule_Lambert(geo_ntf);
}


/**
 * Transform WGS84 lat/lon data to Lambert 93 coordinates.
 */
coord_lamb Wgs84geo_to_Lambert93(coord_geo pos) {

    return calcule_Lambert93(pos);
}


 
