/**
 * \file coordonnees.h
 * \brief Geographical Coordinates Transformations
 * \since 11/04/2007
 *
 * Various Geodesic systems can be used in this project, and the
 * coordinates may have to be transposed from one to another.
 *
 * For each geodesic system, coordinates can be of three forms :
 * cartesian (X,Y,Z), geographical(latitude,longitude,altitude),or
 * plate projection. This module have functions to convert cartesian
 * coordinates to and from geographical coordinates. But not all
 * projections can be handled. The aim of this module is not to be
 * comprehensive.
 * 
 * In the general case geographical coordinates (lat/lon) must be
 * considered to be in the WGS84 system (EPSG 4326). 
 *
 * Some datasets or maps drawing may used Lambert's projection
 * coordinates. Lambert's projections are conical projections for
 * metropolitan France. Two Lambert's projection can be used for entire
 * metropolitan France plus Corsica :
 *
 * The first one, Extensive Lambert II (EPSG 27572), is associated with
 * NTF geodesic system and Clarke's 1880 IGN ellipsoid.
 *
 * The second one is the Lambert 93 projection (EPSG 2154 : new french
 * geodesic system RGF93) which uses the same reference ellipsoid than
 * internationnal WGS84.
 *
 * Transformation between geodesic systems using different ellipsoids
 * must be done in cartesian coordinates. This module has internals
 * functions to change between EPSG 4326 and EPSG 27572.
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
 * Apply the Extended Lambert II projection of geographical coordinates
 * of the NTF system (Clarke's geoid).
 * \param coord geographical coordinate of a point
 * \return the plane coordinates after the projection
 */
coord_lamb calcule_Lambert(coord_geo coord);


/**
 * Apply the Lambert 93 projection of geographical coordinates of the
 * RGF93 system.
 * \param coord geographical coordinate of a point
 * \return the plane coordinates after the projection
 */
coord_lamb calcule_Lambert93(coord_geo coord);


/**
 * Transform WGS84 lat/lon data to Extended Lambert II coordinates.
 */
coord_lamb Wgs84geo_to_Lambert(coord_geo pos);


/**
 * Transform WGS84 lat/lon data to Lambert 93 coordinates.
 */
coord_lamb Wgs84geo_to_Lambert93(coord_geo pos);


#endif
