# Cartograph
Command line tools to play with geographic data.

This project is still experimental.

## Dependencies

This project use [yder](https://github.com/babelouest/yder) as logger module.

So use the following command to clone the repository:

```shell
$ git clone --recurse-submodules https://github.com/thaddeus002/cartograph.git
```

## Examples

### Use bln for image background (program draw_background.c)

For this example, data layer is provide as a png image by a WMS server. The temperatures are placed on an image with a black background.

To localize the data we need a background image with at least countries boundaries. We have this boundaries in file [europe.bln](data/europe.bln).

data source : [Météo France](https://donneespubliques.meteofrance.fr/?fond=geoservices&id_dossier=14)

[geoservices' documentation (in french)](https://donneespubliques.meteofrance.fr/client/gfx/utilisateur/File/documentation-webservices-inspire.pdf)

The raw image is :

![raw_fr](assets/observations.png)

The program output shows the same data with a better background :

![bln_fr](assets/background_observations.png)

### BLN data representation

The program `trace` create a graphic representation of the data of file [world.bln](data/world.bln).

![world](assets/world.png)
