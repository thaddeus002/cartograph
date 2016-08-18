
#ifndef $DESTDIR
DESTDIR=/usr/local
#fi

BINDIR=$(DESTDIR)/bin
#DATADIR=$(DESTDIR)/share/yannview

OBJS=fenetre.o affichage.o lecture_csv.o lecture_grille.o lecture_bln.o outils.o coordonnees.o yImage.o yXinterface.o ySaveImage.o

all: carte trace pointe


carte: carte.c $(OBJS)
	@echo "liaison en" $@
	gcc -Wall carte.c $(OBJS) -o $@ -L/usr/X11R6/lib -lX11 -lm -lz -lbz2 -lpng -ljpeg -ltiff

trace: trace.c $(OBJS)
	@echo "liaison en" $@
	gcc -Wall trace.c $(OBJS) -o $@ -L/usr/X11R6/lib -lX11 -lm -lz -lbz2 -lpng -ljpeg -ltiff

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


# ajout des shapes - en test
lit: lit.c shape_data.o  shape_types.o outils.o fenetre.o
	gcc -o lit lit.c shape_data.o shape_types.o fenetre.o outils.o -L/usr/X11R6/lib -lX11 -lm


