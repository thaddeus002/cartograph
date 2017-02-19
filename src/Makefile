
#ifndef $DESTDIR
DESTDIR=/usr/local
#fi

BINDIR=$(DESTDIR)/bin
#DATADIR=$(DESTDIR)/share/yannview

YDER_LOCATION=../lib/yder/src
YDER_FLAGS=-I$(YDER_LOCATION) -L$(YDER_LOCATION)

OBJS=fenetre.o affichage.o lecture_csv.o lecture_grille.o lecture_bln.o bln.o outils.o coordonnees.o yImage.o yXinterface.o ySaveImage.o map.o yColor.o

PROG=carte trace pointe



all: $(PROG)

libyder:
	make -C $(YDER_LOCATION)


carte: carte.c $(OBJS)
	@echo "liaison en" $@
	gcc -Wall carte.c $(OBJS) -o $@ -L/usr/X11R6/lib -lX11 -lm -lz -lbz2 -lpng -ljpeg -ltiff

trace: trace.c libyder $(OBJS)
	@echo "liaison en" $@
	gcc -Wall trace.c $(OBJS) -o $@ -L/usr/X11R6/lib -lX11 -lm -lz -lbz2 -lpng -ljpeg -ltiff $(YDER_FLAGS) -lyder

pointe: pointe.c $(OBJS)
	@echo "liaison en" $@
	gcc -Wall pointe.c $(OBJS) -o $@ -L/usr/X11R6/lib -lX11 -lm -lz -lbz2 -lpng -ljpeg -ltiff


%.o: %.c %.h
	@echo "compilation de" $<
	gcc -Wall -c $<


install:
	install -c -s carte trace $(BINDIR)
	#mkdir -p $(DATADIR)


clean:
	rm -f core *~ *.o

mrproper: clean
	rm -rf $(PROG)

# ajout des shapes - en test
lit: lit.c shape_data.o  shape_types.o outils.o fenetre.o
	gcc -o lit lit.c shape_data.o shape_types.o fenetre.o outils.o -L/usr/X11R6/lib -lX11 -lm

draw_background: draw_background.c bln.o map.o yImage.o yColor.o ySaveImage.o outils.o
	gcc -o $@ $< bln.o map.o yImage.o yColor.o ySaveImage.o outils.o -lm -lz -lpng

trace_and: trace_and.c map.o yImage.o yColor.o ySaveImage.o outils.o
	gcc -o trace_and trace_and.c map.o yImage.o yColor.o ySaveImage.o outils.o -lm -lz -lpng

libmap: map.o yImage.o yColor.o ySaveImage.o outils.o
	# Static library
	rm -f libmap.a
	ar -cvq libmap.a map.o yImage.o yColor.o ySaveImage.o outils.o
	ranlib libmap.a
