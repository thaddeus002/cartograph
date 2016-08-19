/**
 * \file trace_fond_mf.c
 *
 * Test for map.c module
 */



#include "yImage.h"
#include "ySaveImage.h"
#include "map.h"

// BBOX
#define LATMIN 41
#define LATMAX 52
#define LONMIN -5
#define LONMAX 10

// IMAGE SIZE
#define WIDTH 800
#define HEIGHT 800

#define BOUNDARIES_BLN "../data/europe.bln"



int main(int argc, char **argv) {

    map_t *map;
    yColor backColor, countriesColor;
    int err = 0;

    y_set_color(&backColor, 100, 100, 240, 255);
    y_set_color(&countriesColor, 240, 240, 100, 255);


    map = map_init(EPSG_4326, LATMIN, LONMIN, LATMAX, LONMAX, WIDTH, HEIGHT);
    map_set_background(map, &backColor);

    map_trace_bln(map, BOUNDARIES_BLN, &countriesColor);

    err = sauve_png(map->image, "test_mf.png");
    //sauve_ppm(map->image, "test_mf.ppm");

    map_destroy(map);
    return err;
}

