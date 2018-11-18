
#ifndef $DESTDIR
DESTDIR=/usr/local
#fi

BINDIR=$(DESTDIR)/bin

YDER_LOCATION=../lib/yder/src
YDER_FLAGS=-I$(YDER_LOCATION) -L$(YDER_LOCATION)
YIMAGE_LOCATION=../lib/yImage
YIMAGE_FLAGS=-I$(YIMAGE_LOCATION) -L$(YIMAGE_LOCATION)

OBJS=window.o fenetre.o affichage.o lecture_csv.o points.o lecture_grille.o lecture_bln.o bln.o outils.o coordinates.o yXinterface.o map.o

PROG=carte trace pointe

CFLAGS=-L/usr/X11R6/lib -lm -lz -lbz2 -lpng -ljpeg -ltiff $(YIMAGE_FLAGS) -lyImage

all: $(PROG)

libyder:
	make -C $(YDER_LOCATION)

libyImage:
	make -C $(YIMAGE_LOCATION)

carte: carte.c libyImage $(OBJS)
	@echo "linking in" $@
	gcc -Wall $< $(OBJS) -o $@ $(CFLAGS) -lX11

trace: trace.c libyImage libyder bln.o map.o points.o
	@echo "linking in" $@
	gcc -Wall $< bln.o map.o points.o -o $@ $(CFLAGS) $(YDER_FLAGS) -lyder

trace_x: trace_x.c libyImage libyder $(OBJS)
	@echo "linking in" $@
	gcc -Wall $< $(OBJS) -o $@ $(CFLAGS) $(YDER_FLAGS) -lyder -lX11

pointe: pointe.c libyImage bln.o map.o points.o
	@echo "linking in" $@
	gcc -Wall $< bln.o map.o points.o -o $@ $(CFLAGS)

pointe_x: pointe_x.c libyImage libyder $(OBJS)
	@echo "linking in" $@
	gcc -Wall $< $(OBJS) -o $@ $(CFLAGS) $(YDER_FLAGS) -lyder -lX11


%.o: %.c %.h
	@echo "building of" $<
	gcc -Wall -I$(YIMAGE_LOCATION) -c $<


# TESTS

test_coordinates: test_coordinates.c coordinates.o
	gcc -o $@ test_coordinates.c coordinates.o -lm


install:
	install -c -s carte trace $(BINDIR)
	#mkdir -p $(DATADIR)


clean:
	rm -f core *~ *.o

mrproper: clean
	rm -rf $(PROG) *.a

# ajout des shapes - en test
lit: lit.c shape_data.o  shape_types.o outils.o fenetre.o
	gcc -o lit lit.c shape_data.o shape_types.o fenetre.o outils.o -L/usr/X11R6/lib -lX11 -lm

draw_background: draw_background.c libmap.a
	gcc -o $@ $< -I$(YIMAGE_LOCATION) -L. -lmap -lm -lz -lpng


trace_and: trace_and.c libmap.a
	gcc -o $@ $< -I$(YIMAGE_LOCATION) -L. -lmap -lm -lz -lpng

libmap.a: map.o bln.o points.o libyImage
	# Static library
	rm -f libmap.a
	ar -x $(YIMAGE_LOCATION)/libyImage.a
	ar -cvq libmap.a map.o bln.o points.o y*.o
	ranlib libmap.a
