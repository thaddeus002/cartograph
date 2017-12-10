/**
 * \file points.h
 * \brief Read utility for "points CSV files".
 *
 * These text files contents x, y (may be lambert coordinates or lat lon),
 * name (of the city or whatever object is pointed), optionnal info
 * (department number, ...).
 */

#ifndef __POINTS_H__
#define __POINTS_H__ 1


/**
 * A pointed objet
 */
typedef struct poste_ {
    char commune[250]; /**< object name */
    float X, Y; /**< coordinates */
    int departement; /** department number if it exists */
    struct poste_ *next;
} poste_t;


/**
 * boudaries a points list
 */
typedef struct {
    float xmin, xmax, ymin, ymax;
} data_boundaries_t;

/**
 * \brief Read the next entry in a "points file".
 * \param fd a descriptor on a file
 * \param err
 * \return NULL if reading fail
 */
poste_t *lit_enregistrement_csv(FILE *fd, int *err);


/**
 * \brief read all the points of a "point file".
 */
poste_t *read_points_file(char *csvDataFile);


/**
 * \brief calculate the boundaries of a points list.
 * \param list the points list
 * \param bound must be allocated
 * \return 0 in case of success
 */
int find_data_boundaries(poste_t *list, data_boundaries_t *bound);


#endif
