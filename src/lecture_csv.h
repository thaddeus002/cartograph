/**
 * \file lecture_csv.h
 * \brief Read functions for "points CSV files" and display them in a X11
 * window.
 *
 * These text files contents x, y (may be lambert coordinates or lat lon),
 * name (of the city or whatever object is pointed), optionnal info
 * (department number, ...).
 */

#ifndef __CSV_H__
#define __CSV_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include "fenetre.h"
#include "points.h"



int traite_csv(fenetre fen, char *fichier_cvs, forme_t pointage, int largeur, couleur cpoint, couleur ctexte);


#endif
