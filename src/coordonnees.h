/**
 * \file coordonnees.h
 * \brief transformations de coordonnées geographiques en Lambert II étendu
 * \since 11/04/2007
 **/

#ifndef __COORD_H__
#define __COORD_H__ 1


#include <math.h>


typedef struct {
    float lambda; // longitude en degrés
    float phi; // latitude en degrés
} coord_geo;

typedef struct {
    float X; // en mètres
    float Y; // en mètres
} coord_lamb;

/* fonction de convertion */
coord_lamb calcule_Lambert(coord_geo coord);

/* correction de passage entre les systèmes internationnal et francais */
coord_lamb correction_WGS84(coord_lamb brut);


#endif
