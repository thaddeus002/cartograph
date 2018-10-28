/**
 * \file coordonnees.h
 * \brief Extensive Lambert II Geographical Coordinates Transformations
 * \since 11/04/2007
 *
 * Lambert's projections are conical projections for metropolitan
 * France. In these projections, the points are not located by
 * latitude and longitude coordinates but by plane cartesians (X, Y)
 * coordinates.
 * 
 * Two Lambert's projection can be used for entire metropolitan France
 * plus Corsica.
 *
 * The first one, Extensive Lambert II, is associated with NTF
 * geodesic system and Clarke's 1880 IGN ellipsoid.
 *
 * The second one is the Lambert 93 projection (new french geodesic
 * system RGF93) which uses the same reference ellipsoid than
 * internationnal WGS84.
 **/

#ifndef __COORD_H__
#define __COORD_H__ 1


/**
 * 3D Cartesian coordinates.
 */
typedef struct {
    float X;
    float Y;
    float Z;
} coord_cartesian;


/**
 * Geographical coordinates
 */
typedef struct {
    float lambda; /**< longitude in degrees */
    float phi; /**< latitude in degrees */
    float h;
} coord_geo;


/**
 * Plane coordinates for Lambert's projections 
 */
typedef struct {
    float X; // in meters
    float Y; // in meters
} coord_lamb;


/**
 * Lambert II coordinates calculation from lat/lon of the NTF system
 * (Clarke's geoide).
 */
coord_lamb calcule_Lambert(coord_geo coord);


/**
 * Lambert 93 coordinates calculation from lat/lon of the RGF93 system.
 */
coord_lamb calcule_Lambert93(coord_geo coord);


/**
 * \brief Coordinate translation form WGS84 internationnal system to
 * french NTF.
 */
coord_lamb correction_WGS84(coord_lamb brut);


#endif
