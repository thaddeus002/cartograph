/**
 * \file map.c
 */

#include "map.h"
#include "yImage.h"
#include "yDraw.h"
#include "yText.h"
#include "bln.h"
#include "points.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>



static int linear_transform(int length, float x1, float x2, float x) {

    float d; // Delta x between 2 pixels
    float e; // error between theorical x value (at the center of the pixel) and actual value
    float t; // temporary calculation value
    int i; // return value

    d=(x2-x1)/(length-1);
    t=(x-x1)/d;
    i=1+t;
    e=x-(x1+(i-1)*d);
    if(e>d/2) i++; else if(e<-d/2) i--;

    return(i);
}


/**
 * Give the pixel number for a longitude value in
 * EPSG4326 proj.
 * Pixels are numbered from 1 to w (width), left to right.
 */
static int transforme_x(map_t *map, float x){

    int w = map->image->rgbWidth;
    float x1 = map->boundaries.lonMin;
    float x2 = map->boundaries.lonMax;

    return linear_transform(w, x1, x2, x);
}


/**
 * Give the pixel number for a latitude value in
 * EPSG4326 proj.
 * Pixels are numbered from 1 to h (height), top to bottom.
 */
static int transforme_y(map_t *map, float y) {

    int h = map->image->rgbHeight;
    float y1 = map->boundaries.latMin;
    float y2 = map->boundaries.latMax;

    // y is minimum at the top of the map
    return h+1-linear_transform(h,y1,y2,y);
}



int map_intercept(bbox_t *b1, bbox_t *b2){

    int inter = 1;

    // compare point NW of b1 with point SE of b2

    if( (b1->latMax <= b2->latMin) || (b1->lonMax <= b2->latMin) ) {
        inter = 0;
    }

    // compare point NW of b2 with point SE of b1
    if ( inter && ((b2->latMax <= b1->latMin) || (b2->lonMax <= b1->latMin)) ) {
        inter = 0;
    }

    return inter;
}



/**
 * Create a new map with a empty drawing.
 * \return a pointer to a newly allocated structure
 */
map_t *map_init(geodetic_system_t proj, float latMin, float lonMin, float latMax, float lonMax, int width, int height){

    int err;
    map_t *map = malloc(sizeof(map_t));

    map->proj = proj;

    map->boundaries.latMin = latMin;
    map->boundaries.latMax = latMax;
    map->boundaries.lonMin = lonMin;
    map->boundaries.lonMax = lonMax;

    map->image=y_create_image(&err, NULL, width, height);

    return map;
}


/**
 * Free the memory used by a map.
 */
void map_destroy(map_t *map){
    if(map!=NULL) {
        y_destroy_image(map->image);
        free(map);
    }
}


/**
 * Copy the image background on the map.
 * \return the pointer to the new image
 */
yImage *map_set_image(map_t *map, yImage *image){

    int err;
    yImage *img = y_create_image(&err, image->rgbData, image->rgbWidth, image->rgbHeight);

    if(map->image!=NULL) {
        y_destroy_image(map->image);
    }

    map->image = img;

    return img;
}


/**
 * Copy the color background on the map.
 * \return the pointer to the map image
 */
yImage *map_set_background(map_t *map, yColor *color){

    int err;
    yImage *img = y_create_uniform_image(&err, color, map->image->rgbWidth, map->image->rgbHeight);

    y_destroy_image(map->image);

    map->image = img;

    return img;
}



/**
 * Add lines or polygons to the map.
 */
int map_trace_bln_data(map_t *map, bln_data_t *blnData, yColor *color, yColor *fillColor){
    /* variables */
    int i; /* number of the current coordinate line */
    yPoint *points; /* list of points to link */
    bln_data_t *current; /* to cross the data */

    /* initialisations */
    i=0;
    current=blnData;

    while(current!=NULL){

        points=malloc(current->nbPoints*sizeof(yPoint));

        for(i=0; i<current->nbPoints; i++) {

            points[i].X=transforme_x(map, current->x[i]);
            points[i].Y=transforme_y(map, current->y[i]);
        }

        #ifdef DEBUG
        fprintf(stdout, "drawing for %d points\n", current->nbPoints);
        #endif

        if(fillColor != NULL) {
            y_fill_polygon(map->image, fillColor, points, current->nbPoints);
        }

        y_draw_lines(map->image, color, points, current->nbPoints);

        free(points);

        current = current->next;
    }

    return(0);
}


/**
 * Add lines or polygons to the map without fill them.
 */
int map_trace_bln_data_empty(map_t *map, bln_data_t *blnData, yColor *color){
    return map_trace_bln_data(map, blnData, color, NULL);
}



map_t *map_create_with_bln(char *blnFile, yColor *background, yColor *color, yColor *fillColor, geodetic_system_t proj, int width, int height){

    bln_data_t *data = bln_read_file(blnFile);
    map_t *map = NULL;

    if(data != NULL) {
        bln_boundaries_t bound;
        bln_find_data_boundaries(data, &bound);
        fprintf(stdout, "Map's Boundaries : %f,%f - %f,%f\n", bound.xmin, bound.xmax, bound.ymin, bound.ymax);
        map = map_init(proj, bound.ymin, bound.xmin, bound.ymax, bound.xmax, width, height);
        map_set_background(map, background);
        map_trace_bln_data(map,data,color,fillColor);
        bln_destroy(data);
    }

    return map;
}



static void map_draw_line(map_t *map, float lon1, float lon2, float lat1, float lat2, yColor *color) {

    yPoint M, N;

    M.X=transforme_x(map, lon1);
    M.Y=transforme_y(map, lat1);
    N.X=transforme_x(map, lon2);
    N.Y=transforme_y(map, lat2);

    y_draw_line(map->image, M, N, color);
}



void map_draw_meridians(map_t *map, yColor *color) {

    int i;

    // draw equator, tropics and polar circles
    map_draw_line(map, map->boundaries.lonMin, map->boundaries.lonMax, 0, 0, color);
    map_draw_line(map, map->boundaries.lonMin, map->boundaries.lonMax, 23.45, 23.45, color);
    map_draw_line(map, map->boundaries.lonMin, map->boundaries.lonMax, -23.45, -23.45, color);
    map_draw_line(map, map->boundaries.lonMin, map->boundaries.lonMax, 66.55, 66.55, color);
    map_draw_line(map, map->boundaries.lonMin, map->boundaries.lonMax, -66.55, -66.55, color);

    /* draw some meridians */
    for(i=-120;i<=180;i=i+60){
        map_draw_line(map, i, i, map->boundaries.latMin, map->boundaries.latMax, color);
    }
}



static int pointe(map_t *map, float xLamb, float yLamb, int L, yColor *c, shape_t forme) {
    int x, y; // coordinates on map
    int k, l; // counters
    yPoint p;

    /* map coordinates to image coordinates */
    x=transforme_x(map, xLamb);
    y=transforme_y(map, yLamb);

    if(x<0) return(1);
    if(y<0) return(1);
    if((x>map->image->rgbWidth)||(y>map->image->rgbHeight)) return(2);

    if(L==0) forme=SQUARE;

    if (forme==SQUARE) {
        for(k=-L; k<=L; k++) {
            for(l=-L; l<=L; l++){
                p.X=x+l;
                p.Y=y+k;
                y_draw_point(map->image, p, c);
            }
        }
    } else if(forme==TRIANGLE) {
        for(k=-L; k<=L+1; k++) {
            for(l=-(k+L)*2/3; l<=(k+L)*2/3; l++) {
                p.X=x+l;
                p.Y=y+k;
                y_draw_point(map->image, p, c);
            }
        }
    } else if(forme==ROUND) {
        for(k=-L; k<=L; k++) {
            for(l=-floor(sqrt(L*L*10000-10000*k*k)/100); l<floor(sqrt(10000*L*L-10000*k*k)/100); l++) {
                p.X=x+l;
                p.Y=y+k;
                y_draw_point(map->image, p, c);
            }
        }
    }

    return(0);
}




static int display_text(yImage *image, int x, int y, char *text, yColor *c){
    char line[80]; // string to show
    char *ptr1, *ptr2; // to split the string in lines
    int i; // line's number
    int n; // number of characters to show on line i (before line feed)
    int err;

    i = 0;

    ptr1 = text;
    while (ptr1) {
        ptr2 = index(ptr1, '\n');
        if (!ptr2)
            strcpy(line, ptr1);
        else {
            n = ptr2 - ptr1;
            strncpy(line, ptr1, n);
            line[n] = '\0';
        }

        err = y_display_text_with_color(image, x, y+24*i, line, c);

        ++i;


        if (ptr2) {
            ptr1 = ptr2+1;
        } else {
            ptr1 = NULL;
        }
    }

    return err;
}



static int pointe_ville(map_t *map, poste_t *ville, shape_t pointage, int largeur, yColor *cpoint, yColor *ctext){
    int p; // err code

    p=pointe(map, ville->X, ville->Y, largeur, cpoint, pointage);

    if(p==0) {
        p=display_text(map->image, transforme_x(map, ville->X)+9, transforme_y(map, ville->Y), ville->commune, ctext);
    }

    return(p);
}


int map_point(map_t *map, poste_t *points, shape_t pointage, int largeur, yColor *cpoint, yColor *ctext){

    poste_t *courant;
    int err;

    courant = points;
    while(courant != NULL) {
        err=pointe_ville(map, courant, pointage, largeur, cpoint, ctext);
        courant = courant->next;
    }

    return(err);
}
