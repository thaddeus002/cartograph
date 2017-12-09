/**
 * \file draw_background.c
 *
 * Example program using map.c module.
 * We have a map with some data values pointed on it. But this map's
 * background is black. This map was retreived on a WMS server in
 * "meteofrance.com". It represent air temperatures data on some points
 * in metropolitan France and Corsica. To make this map more usefull, we
 * need to change the background to one that make understandable where
 * are the measures from. A "bln" file of Europe will be a good help for
 * this.
 */


#include "yImage.h"
#include "yImage_io.h"
#include "map.h"
#include "bln.h"


// BBOX : The same as used for the WMS request
#define LATMIN 41
#define LATMAX 52
#define LONMIN -5
#define LONMAX 10

// IMAGE SIZE
#define WIDTH 800
#define HEIGHT 800

#define BOUNDARIES_BLN "../data/europe.bln"


#define LAYER_PNG "../data/pointage_temperatures_fr.png"

int main(int argc, char **argv) {

    map_t *map;
    yColor backColor, countriesColor, landColor;
    int err = 0;
    yImage *pointage;
    bln_data_t *boundaries;

    y_set_color(&backColor, 100, 100, 240, 255);
    y_set_color(&countriesColor, 240, 240, 100, 255);
    y_set_color(&landColor, 0, 200, 0, 255);

    map = map_init(EPSG_4326, LATMIN, LONMIN, LATMAX, LONMAX, WIDTH, HEIGHT);
    map_set_background(map, &backColor);

    boundaries = bln_read_file(BOUNDARIES_BLN);
    if(boundaries != NULL) {
        map_trace_bln_data(map, boundaries, &countriesColor, &landColor);
        bln_destroy(boundaries);
    }


    pointage=y_load_png(LAYER_PNG);
    if(pointage != NULL) {
        y_grey_level_to_alpha(pointage);
        y_superpose_images(map->image, pointage, 0, 0);
        y_destroy_image(pointage);
    }

    err = y_save_png(map->image, "test_mf.png");

    map_destroy(map);
    return err;
}

