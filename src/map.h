/**
 * \file map.h
 * \brief permit drawing on a image considering it as a map.
 *
 * Transform an yImage representation in a map representation adding
 *  - projection information : only EPSG:4326 will be first supported.
 *    this projection is also name "platte-carree" and is very simple.
 *    (x,y) coordinates on the map are linear functions of (lon,lat).
 *  - the bounds coordinates, like in BBOX: LonMin,LatMin,LogMax,LatMax
 */



#include "yImage.h"
#include "bln.h"


#ifndef __Y_MAP_H__
#define __Y_MAP_H__



/**
 * Supported projections
 */
typedef enum {
    EPSG_4326
} yProjection;


/**
 * Boundaries box
 */
typedef struct bbox_ {
    float latMin, lonMin, latMax, lonMax;
} bbox_t;



/**
 * The map.
 */
typedef struct map_ {

    yImage *image; /**< the image representing the map */
    yProjection proj; /**< the projection used */
    bbox_t boundaries; /**< the coordinates in the boundaries of the image */
} map_t;


/**
 * Is there an intercetion between two bboxes? Common boundaries are NOT considered as interception.
 * \return 0 if the interception between the 2 bbox is empty.
 */
int map_intercept(bbox_t *b1, bbox_t *b2);



/**
 * Create a new map with a empty drawing.
 * \return a pointer to a newly allocated structure
 */
map_t *map_init(yProjection proj, float latMin, float lonMin, float latMax, float lonMax, int width, int height);


/**
 * Free the memory used by a map.
 */
void map_destroy(map_t *map);


/**
 * Copy the image background on the map.
 * \return the pointer to the new image
 */
yImage *map_set_image(map_t *map, yImage *image);


/**
 * Copy the color background on the map.
 * \return the pointer to the map image
 */
yImage *map_set_background(map_t *map, yColor *color);



/**
 * Add lines or polygons to the map.
 */
int map_trace_bln(map_t *map, char *blnFile, yColor *color);

/**
 * Add lines or polygons to the map.
 */
int map_trace_bln_data(map_t *map, bln_data_t *blnData, yColor *color);

/**
 * \brief Create a new map with the drawing of bln.
 * \param blnFile data file's name
 * \param background the color for the background of the map
 * \param color the color for the data drawing
 * \param proj the geographic projection to use
 * \param width the map's width
 * \param height the map's height
 * \return the created map
 */
map_t *map_create_with_bln(char *blnFile, yColor *background, yColor *color, yProjection proj, int width, int height);


#endif
