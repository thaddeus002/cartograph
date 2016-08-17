/**
 * \file coordonnees.h
 * \brief transformations de coordonn�es geographiques en Lambert II �tendu
 * \since 11/04/2007
 **/

#ifndef __COORD_H__
#define __COORD_H__ 1


#include <math.h>


typedef struct {
    float lambda; // longitude en degr�s
    float phi; // latitude en degr�s
} coord_geo;

typedef struct {
    float X; // en m�tres
    float Y; // en m�tres
} coord_lamb;

/* fonction de convertion */
coord_lamb calcule_Lambert(coord_geo coord);

/* correction de passage entre les syst�mes internationnal et francais */
coord_lamb correction_WGS84(coord_lamb brut);


#endif
