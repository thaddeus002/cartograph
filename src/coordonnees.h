/**
 * \file coordonnees.h
 * \brief Geographical Coordinates Transformations
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


// Representing the coordinates

/**
 * Enumeration of coordinates types.
 */
typedef enum {
    CARTESIAN,
    GEOGRAPHICAL,
    PLANE
} coordinates_t;


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
    float h; /**< altitude */
} coord_geo;


/**
 * Plane coordinates. For Lambert's projections there are in meters. 
 */
typedef struct {
    float X;
    float Y;
} coord_plane;


// Making projections : geographical to plane coordinates

/**
 * Apply the Extended Lambert II projection on geographical coordinates.
 * The geogragraphical coordinates are supposed to be in the NTF system
 * (Clarke's geoid).
 * \param coord geographical coordinate of a point
 * \return the plane coordinates after the projection
 */
coord_plane proj_Lambert(coord_geo coord);


/**
 * Apply the Lambert 93 projection of geographical coordinates. The
 * geographical coordinates are supposed to be in the RGF93 system.
 * \param coord geographical coordinate of a point
 * \return the plane coordinates after the projection
 */
coord_plane proj_Lambert93(coord_geo coord);


/**
 * Apply the "Plate Caree" projection of geographical coordinates.
 * This is the projection used in WGS84 system.
 * \param coord geographical coordinate of a point
 * \return the plane coordinates after the projection
 */
coord_plane proj_PlateCaree(coord_geo coord);


// plane to geographical coordinate


/**
 * Extended Lambert II projection to geographical coordinates.
 * The geogragraphical coordinates are in the NTF system
 * (Clarke's geoid).
 * \param coord Lambert coordinates of a point
 * \return the corresponding geographical coordinates
 */
coord_geo Lambert_to_geographical(coord_plane coord);


/**
 * Lambert 93 to geographical coordinates.
 * The geographical coordinates are in the RGF93 system.
 * \param coord Lambert93 coordinates of a point
 * \return the corresponding geographical coordinates
 */
coord_geo Lambert93_to_geographical(coord_plane coord);


/**
 * Revert a "Plate Caree" projection to geographical coordinates.
 * \param coord plane coordinate of a point in plate caree projection
 * \return the geographical coordinate of the point
 */
coord_geo plate_caree_to_geographical(coord_plane coord);


// Switching between geodics systems


/**
 * Transform WGS84 to Extended Lambert II coordinates.
 */
coord_plane Wgs84_to_Lambert(coord_plane pos);


/**
 * Transform WGS84 to Lambert 93 coordinates.
 */
coord_plane Wgs84_to_Lambert93(coord_plane pos);


#endif
