/**
 * \file coordinates.c
 * \brief Extensive Lambert II Geographical Coordinates Transformations
 * \since 11/04/2007
 */

#include "coordinates.h"

#include <math.h>


/* constantes de projection Lambert II �tendu */
#define C_N 0.7289686274
#define C_C 11745793.39
#define C_XS 600000
#define C_YS 8199695.768

/* excentricite de l'ellipsoide */
#define C_E 0.08248325676
/* longitude du m�ridien central : 2�20'14,025'' E de Greenwich */
# define C_L0 2.3372291667


/* constantes de projection Lambert 93 -> g�oide diff�rent (RGF93)*/
#define C93_N 0.7256077650
#define C93_C 11754255.426
#define C93_XS 700000
#define C93_YS 12655612.05
/* uses 3�E from Greenwich meridian */
#define C93_L0 3
// TODO must find the real excentricity
#define C93_E C_E


/** Converts degrees to radians */
static float radians(float degres){ return (degres*M_PI/180); }

/** Converts radians to degrees */
static float degrees(float radians) { return (radians*180/M_PI); }


/**********************************************************//**
 *    GEOGRAPHICAL (Lambda,Phy,h) TO PLANE (X,Y)              *
 *************************************************************/


/**
 * Calculate Lambert coordinate from lat/lon of the same geodesic
 * system.
 */
static coord_plane calculate_plane_coordinates(coord_geo coord, double e, double c, double n, double l0, double xs, double ys) {
    coord_plane result; /* return value */

    /* intermediary calculations */
    double S=sinf(radians(coord.phi));
    double L=0.5*logf( (1+S)/(1-S) ) - 0.5*e*logf( (1+e*S)/(1-e*S) );
    double R=c*expf(-n*L);
    double gamma=n*(coord.lambda-l0);

    /* result */
    result.X=xs+R*sinf(radians(gamma));
    result.Y=ys-R*cosf(radians(gamma));

    return(result);
}


/**
 * Apply the Extended Lambert II projection on geographical coordinates.
 * The geogragraphical coordinates are supposed to be in the NTF system
 * (Clarke's geoid).
 * \param coord geographical coordinate of a point
 * \return the plane coordinates after the projection
 */
coord_plane proj_Lambert(coord_geo coord) {
    return calculate_plane_coordinates(coord, C_E, C_C, C_N, C_L0, C_XS, C_YS);
}


/**
 * Apply the Lambert 93 projection of geographical coordinates. The
 * geographical coordinates are supposed to be in the RGF93 system.
 * \param coord geographical coordinate of a point
 * \return the plane coordinates after the projection
 */
coord_plane proj_Lambert93(coord_geo coord) {
    return calculate_plane_coordinates(coord, C93_E, C93_C, C93_N, C93_L0, C93_XS, C93_YS);
}


coord_plane proj_PlateCaree(coord_geo coord) {
    coord_plane result;
    // this projection is very simple
    result.X = coord.lambda;
    result.Y = coord.phi;
    return result;
}



/**********************************************************//**
 *         PLANE (X,Y) TO GEOGRAPHICAL (Lambda,Phy,h)         *
 *************************************************************/


static double iter_pi(double pi, double L, double e) {
    return 2*atan(pow((1+e*sin(pi))/(1-e*sin(pi)),e/2)*exp(L))-M_PI/2;
}

static double lat_from_isometric_lat(double L, double e) {

    double p0 = 2*atan(exp(L))-M_PI/2;

    double pi = p0;
    double pj = iter_pi(pi, L, e);

    while(fabs(pj-pi)>1e-11) {
        pi = pj;
        pj = iter_pi(pi, L, e);       
    }

    return pj;
}


static coord_geo calculate_geo_coordinates(coord_plane coord, double e, double c, double n, double l0, double xs, double ys) {
    coord_geo result; /* return value */

    double R = sqrt((coord.X-xs)*(coord.X-xs)+(coord.Y-ys)*(coord.Y-ys));
    double gamma = atan((coord.X-xs)/(ys-coord.Y));
    result.lambda=degrees(radians(l0)+gamma/n);
    double L = -1/n * log(fabs(R/c));
    result.phi=degrees(lat_from_isometric_lat(L, e));

    return result;
}


/**
 * Extended Lambert II projection to geographical coordinates.
 * The geogragraphical coordinates are in the NTF system
 * (Clarke's geoid).
 * \param coord Lambert coordinates of a point
 * \return the corresponding geographical coordinates
 */
coord_geo Lambert_to_geographical(coord_plane coord) {
    return calculate_geo_coordinates(coord, C_E, C_C, C_N, C_L0, C_XS, C_YS);
}


/**
 * Lambert 93 to geographical coordinates.
 * The geographical coordinates are in the RGF93 system.
 * \param coord Lambert93 coordinates of a point
 * \return the corresponding geographical coordinates
 */
coord_geo Lambert93_to_geographical(coord_plane coord) {
    return calculate_geo_coordinates(coord, C93_E, C93_C, C93_N, C93_L0, C93_XS, C93_YS);
}


/**
 * Revert a "Plate Caree" projection to geographical coordinates.
 * \param coord plane coordinate of a point in plate caree projection
 * \return the geographical coordinate of the point
 */
coord_geo plate_caree_to_geographical(coord_plane coord){
    coord_geo result;
    result.lambda = coord.X;
    result.phi = coord.Y;
    result.h = 0;
    return result;
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
 * Transform WGS84 to Extended Lambert II coordinates.
 */
coord_plane Wgs84_to_Lambert(coord_plane pos) {

    coord_geo geo_wgs = plate_caree_to_geographical(pos);
    coord_cartesian cartesian_wgs = geo2cartesian(geo_wgs, WGS_A, WGS_B);
    coord_cartesian cartesian_ntf = wgs2ntf(cartesian_wgs);
    coord_geo geo_ntf = cartesian2geo(cartesian_ntf, NTF_A, NTF_B);
    return proj_Lambert(geo_ntf);
}


/**
 * Transform WGS84 to Lambert 93 coordinates.
 */
coord_plane Wgs84_to_Lambert93(coord_plane pos) {

    coord_geo geo_wgs = plate_caree_to_geographical(pos);
    // the geographical coordinates are the same for WGS84 and Lambert93
    // because of the use of the same reference ellipsoid.
    return proj_Lambert93(geo_wgs);
}


/**
 * Transform Extended Lambert II coordinates to WGS84.
 */
coord_plane Lambert_to_Wgs84(coord_plane pos) {

    coord_geo geo_lamb = Lambert_to_geographical(pos);
    coord_cartesian cartesian_ntf = geo2cartesian(geo_lamb, NTF_A, NTF_B);
    coord_cartesian cartesian_wgs = ntf2wgs(cartesian_ntf);
    coord_geo geo_wgs = cartesian2geo(cartesian_wgs, WGS_A, WGS_B);
    return proj_PlateCaree(geo_wgs);
}


/**
 * Transform Lambert93 coordinates to WGS84.
 */
coord_plane Lambert93_to_Wgs84(coord_plane pos) {

    coord_geo geo_lamb = Lambert93_to_geographical(pos);
    // the geographical coordinates are the same for WGS84 and Lambert93
    // because of the use of the same reference ellipsoid.
    return proj_PlateCaree(geo_lamb);
}


coord_plane coordinates_convert(coord_plane pos, geodetic_system_t from, geodetic_system_t to) {

    coord_plane result = { .X=pos.X, .Y=pos.Y };

    if(to == from) {
        return result;
    }

    coord_plane posl;
    if(from!=EPSG_4326) {
        // Lambert data are in hectometers
        // TODO must find a better way to do this
        posl.X=pos.X*100;
        posl.Y=pos.Y*100;
    }

    coord_geo geo0;
    double a0, b0;
    switch(from) {
    case EPSG_4326 :
        geo0 = plate_caree_to_geographical(pos);
        a0 = WGS_A;
        b0 = WGS_B;
        break;
    case EPSG_27572:
        geo0 = Lambert_to_geographical(posl);
        a0 = NTF_A;
        b0 = NTF_B;
        break;
    case EPSG_2154:
        geo0 = Lambert93_to_geographical(posl);
        a0 = WGS_A;
        b0 = WGS_B;
        break;
    }

    coord_cartesian cartesian0 = geo2cartesian(geo0, a0, b0);
    coord_cartesian cartesian1 = { .X=cartesian0.X, .Y=cartesian0.Y };

    if ((from == EPSG_4326 || from == EPSG_2154) && (to == EPSG_27572)) {
        cartesian1 = wgs2ntf(cartesian0);
    } else if ((from == EPSG_27572) && (to == EPSG_4326 || to == EPSG_2154)) {
        cartesian1 = ntf2wgs(cartesian0);
    }

    double a1, b1;
    coord_plane (*proj)(coord_geo);
    switch(to) {
    case EPSG_4326 :
        a1 = WGS_A;
        b1 = WGS_B;
        proj = proj_PlateCaree;
        break;
    case EPSG_27572:
        a1 = NTF_A;
        b1 = NTF_B;
        proj = proj_Lambert;
        break;
    case EPSG_2154:
        a1 = WGS_A;
        b1 = WGS_B;
        proj = proj_Lambert93;
        break;
    }
    
    coord_geo geo1 = cartesian2geo(cartesian1, a1, b1);
    if(from==EPSG_4326 && to!=EPSG_4326) {
        // to have Lambert data in hectometers
        // TODO must find a better way to do this
        coord_plane pos1 = proj(geo1);
        pos1.X=pos1.X/100;
        pos1.Y=pos1.Y/100;
        return pos1;
    }
    return proj(geo1);
}
