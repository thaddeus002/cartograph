/**
 * \file bln.h
 * \brief A BLN file contains informations to draw a line on a map.
 *
 * Each line of file contains the coordinates of a point. Linking the points
 * give a curve : country boundaries, rivers, roads, ...
 */

#ifndef __BLN_FILE_H__
#define __BLN_FILE_H__ 1

#include "coordinates.h"


/**
 * \brief Information a of line. A BLN file may contains many of lines. So this
 * struct may be linked.
 *
 * The first line contains 1 to 4 informations :
 *    - Number of points
 *    - Closed (optionnal) : 0 or 1, indicate if the line is closed (i.e. the last
 * point is the same as the first)
 *    - Name (optional) : for a country this is the name of the country
 *    - Description (optional) : for exemple it can be a region's name
 * Then there are nbPoints line with lon,lat coordinates (it can be x,y
 * in Lambert's coordinates)
 */
typedef struct bln_data_t_ {
    int nbPoints; /**< \brief number of points */
    int closed; /**< \brief not NULL if the line is closed */
    char *name; /**< \brief line name, can be a country name */
    char *description; /**< \brief complementary description, may be a region */
    /* tables of coordinates */
    float *x, *y;
    geodetic_system_t type; /**< \brief geodetic system of x and y coordinates */
    /* boundaries' coordinates */
    float xmin, xmax, ymin, ymax;
    struct bln_data_t_ *next; /**< \brief link to the next structur */
} bln_data_t;


/**
 * \brief The boundaries of a struct bln_data_t_
 * \deprecated this type may be remove in the future
 */
typedef struct {
    float xmin, xmax, ymin, ymax;
    int result; // err code
} bln_boundaries_t;


/* functions */


/**
 * \brief find the boundaries of a data set.
 * \param data the dataset
 * \param bound the address of a result struct. Must be allocated
 * \return 0 in case of success
 */
int bln_find_data_boundaries(bln_data_t *data, bln_boundaries_t *bound);


/**
 * \brief Read a bln file.
 * \param filename
 * \param type the geodetic system of the data
 * \return the content of the file
 */
bln_data_t *bln_read_file(char *filename, geodetic_system_t type);


/**
 * \brief free the memory used by the bln data.
 */
void bln_destroy(bln_data_t *data);


/**
 * \brief Write the data in a file.
 */
int bln_write_file(bln_data_t *data, char *filename);


/**
 * create a new variable to fit a given geodetic system.
 * \param line the data struct to adapt
 * \param type the target geodetic system
 * \return a newly allocated struct or NULL if memory allocation failed
 */
bln_data_t *bln_toGS(bln_data_t *line, geodetic_system_t type);


#endif
