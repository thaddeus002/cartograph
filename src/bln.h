/**
 * \file bln.h
 * \brief A BLN file contains informations to draw a line on a map.
 *
 * Each line of file contains the coordinates of a point. Linking the points
 * give à curve : country boundaries, rivers, roads, ...
 */

#ifndef __BLN_FILE_H__
#define __BLN_FILE_H__ 1


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
    /** tables of coordinates */
    float *x, *y;
    struct bln_data_t_ *next; /**< \brief link to the next structur */
} bln_data_t;


/**
 * \brief The boundaries of a struct bln_data_t_
 */
typedef struct {
    float xmin, xmax, ymin, ymax;
    int result; // err code
} bln_boundaries_t;


/* functions */

bln_boundaries_t *bln_find_boundaries(char *file);



/**
 * \brief Read a bln file.
 * \param filename
 * \return the content of the file
 */
bln_data_t *bln_read_file(char *filename);


/**
 * \brief free the memory used by the bln data.
 */
void bln_destroy(bln_data_t *data);


/**
 * \brief Write the data in a file.
 */
int bln_write_file(bln_data_t *data, char *filename);




#endif
