
#ifndef $DESTDIR
DESTDIR=/usr/local
#fi

BINDIR=$(DESTDIR)/bin
#DATADIR=$(DESTDIR)/share/yannview

YDER_LOCATION=../lib/yder/src
YDER_FLAGS=-I$(YDER_LOCATION) -L$(YDER_LOCATION)
YIMAGE_LOCATION=../lib/yImage
YIMAGE_FLAGS=-I$(YIMAGE_LOCATION) -L$(YIMAGE_LOCATION)

OBJS=fenetre.o affichage.o lecture_csv.o lecture_grille.o lecture_bln.o bln.o outils.o coordonnees.o yXinterface.o map.o

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

trace: trace.c libyImage libyder bln.o map.o
	@echo "linking in" $@
	gcc -Wall $< bln.o map.o -o $@ $(CFLAGS) $(YDER_FLAGS) -lyder

pointe: pointe.c libyImage bln.o map.o
	@echo "linking in" $@
	gcc -Wall $< bln.o map.o -o $@ $(CFLAGS) 


%.o: %.c %.h
	@echo "building of" $<
	gcc -Wall -I$(YIMAGE_LOCATION) -c $<


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
	gcc -o $@ $< -L. -lmap -lm -lz -lpng

trace_and: trace_and.c libmap.a
	gcc -o trace_and trace_and.c -L. -lmap -lm -lz -lpng

libmap.a: map.o yImage.o yColor.o ySaveImage.o bln.o outils.o
	# Static library
	rm -f libmap.a
	ar -cvq libmap.a map.o bln.o yImage.o yColor.o ySaveImage.o
	ranlib libmap.a
