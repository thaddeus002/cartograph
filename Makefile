
all: binaries

binaries:
	make -C src

clean:
	make -C src clean

mrproper:
	make -C src mrproper

doxygen: Doxyfile
	doxygen
