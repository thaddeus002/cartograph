/**
 * \file shape_types.h
 * \brief Types de formes dans les fichiers shapefile
 * \author Yannick Garcia
 * \since 23 mai 2008
 */

#ifndef SHAPE_TYPE
#define SHAPE_TYPE 1

#include <stdlib.h>

typedef enum {NULL_SHAPE=0, POINT=1, POLYLINE=3, POLYGON=5, MULTIPOINT=8, POINTZ=11, POLYLINEZ=13,
POLYGONZ=15, MULTIPOINTZ=18, POINTM=21, POLYLINEM=23, POLYGONM=25, MULTIPOINTM=28, MULTIPATCH=31} shapetype;


/* Entete */
typedef struct {
    int fileCode; /* 9994 */
    int lenght; /* longueur du fichier en demi-octets */
    int version; /* 1000 */
    int shapeType;
    double boundarieBox[8];
} shape_header;


/* Types de données */

typedef struct {
    //int shapeType=0;
} Null_Shape;


typedef struct {
    //int shapeType=1;
    double x;
    double y;
} Point;

typedef struct {
    double box[4]; /* Xmin, Ymin, Xmax, Ymax */
    int numPoints; /* nb de points */
    Point *points;
} MultiPoint;


typedef struct {
    double box[4]; /* Xmin, Ymin, Xmax, Ymax */
    int numParts; /*nb de parts*/
    int numPoints; /* nb de points */
    int *parts; /*index of first point in part*/
    Point *points; /*points of all parts*/
} PolyLine;


typedef struct {
    double box[4]; /* Xmin, Ymin, Xmax, Ymax */
    int numParts; /*nb de parts*/
    int numPoints; /* nb de points */
    int *parts; /*index of first point in part*/
    Point *points; /*points of all parts*/
} Polygon;

typedef struct {
    //int shapeType=1;
    double x;
    double y;
    double m; /*Measure*/
} PointM;

typedef struct {
    double box[4]; /* Xmin, Ymin, Xmax, Ymax */
    int numPoints; /* nb de points */
    Point *points;
    double mRange[2]; /*bounding Measure Range: Mmin, Mmax*/
    double *mArray; /*numPoints measures*/
} MultiPointM;


typedef struct {
    double box[4]; /* Xmin, Ymin, Xmax, Ymax */
    int numParts; /*nb de parts*/
    int numPoints; /* nb de points */
    int *parts; /*index of first point in part*/
    Point *points; /*points of all parts*/
    double mRange[2]; /*bounding Measure Range: Mmin, Mmax*/
    double *mArray; /*numPoints measures*/
} PolyLineM;


typedef struct {
    double box[4]; /* Xmin, Ymin, Xmax, Ymax */
    int numParts; /*nb de parts*/
    int numPoints; /* nb de points */
    int *parts; /*index of first point in part*/
    Point *points; /*points of all parts*/
    double mRange[2]; /*bounding Measure Range: Mmin, Mmax*/
    double *mArray; /*numPoints measures*/
} PolygonM;

typedef struct {
    //int shapeType=1;
    double x;
    double y;
    double z; /*z coordinate*/
    double m; /*Measure*/
} PointZ;

typedef struct {
    double box[4]; /* Xmin, Ymin, Xmax, Ymax */
    int numPoints; /* nb de points */
    Point *points;
    double zRange[2];
    double *zArray; /*Z values*/
    double mRange[2]; /*bounding Measure Range: Mmin, Mmax*/
    double *mArray; /*numPoints measures*/
} MultiPointZ;


typedef struct {
    double box[4]; /* Xmin, Ymin, Xmax, Ymax */
    int numParts; /*nb de parts*/
    int numPoints; /* nb de points */
    int *parts; /*index of first point in part*/
    Point *points; /*points of all parts*/
    double zRange[2];
    double *zArray; /*Z values*/
    double mRange[2]; /*bounding Measure Range: Mmin, Mmax*/
    double *mArray; /*numPoints measures*/
} PolyLineZ;


typedef struct {
    double box[4]; /* Xmin, Ymin, Xmax, Ymax */
    int numParts; /*nb de parts*/
    int numPoints; /* nb de points */
    int *parts; /*index of first point in part*/
    Point *points; /*points of all parts*/
    double zRange[2];
    double *zArray; /*Z values*/
    double mRange[2]; /*bounding Measure Range: Mmin, Mmax*/
    double *mArray; /*numPoints measures*/
} PolygonZ;


typedef struct {
    double box[4]; /* Xmin, Ymin, Xmax, Ymax */
    int numParts; /*nb de parts*/
    int numPoints; /* nb de points */
    int *parts; /*index of first point in part*/
    int *partTypes; /* types des différentes parties */
    Point *points; /*points of all parts*/
    double zRange[2];
    double *zArray; /*Z values*/
    double mRange[2]; /*bounding Measure Range: Mmin, Mmax*/
    double *mArray; /*numPoints measures*/
} MultiPatch;
/* les types sont
0 triangle strip
1 triangle fan
2 outer ring
3 inner ring
4 first ring
5 ring */

/* le type générique pouvant correspondre à n'importe lequel definit précédemment */
typedef struct {
    int shapeType;

    /* les simples points */
    double x;
    double y;
    double z; /*z coordinate*/
    double m; /*Measure*/

    /* les polypoints et autres */
    double box[4]; /* Xmin, Ymin, Xmax, Ymax */
    int numParts; /*nb de parts*/
    int numPoints; /* nb de points */
    int *parts; /*index of first point in part*/
    int *partTypes; /* types des différentes parties */
    Point *points; /*points of all parts*/
    double zRange[2];
    double *zArray; /*Z values*/
    double mRange[2]; /*bounding Measure Range: Mmin, Mmax*/
    double *mArray; /*numPoints measures*/
} GenericShape;

GenericShape *init_shape();

int destroy_shape(GenericShape *sh);


#endif
