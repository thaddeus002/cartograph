/**
 * \file trace_fond_mf.c
 *
 * Test for map.c module
 */



#include "yImage.h"
#include "ySaveImage.h"
#include "map.h"
#include "bln.h"


// BBOX
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
    yColor backColor, countriesColor;
    int err = 0;
    yImage *pointage;
    FILE *fd;
    bln_data_t *boundaries;

    y_set_color(&backColor, 100, 100, 240, 255);
    y_set_color(&countriesColor, 240, 240, 100, 255);


    map = map_init(EPSG_4326, LATMIN, LONMIN, LATMAX, LONMAX, WIDTH, HEIGHT);
    map_set_background(map, &backColor);

    boundaries = bln_read_file(BOUNDARIES_BLN);
    if(boundaries != NULL) {
        map_trace_bln_data(map, boundaries, &countriesColor);
        bln_destroy(boundaries);
    }


    fd=fopen(LAYER_PNG, "r");
    if(fd!=NULL) {
        pointage=LoadPNG(fd);
        fclose(fd);

        // replace black by transparency
        //pointage->presShapeColor=1;
        //y_set_color(&(pointage->shapeColor), 0, 0, 0, 255);
        y_grey_level_to_alpha(pointage);

        superpose_images(map->image, pointage, 0, 0);
    }

    err = sauve_png(map->image, "test_mf.png");


    map_destroy(map);
    return err;
}

