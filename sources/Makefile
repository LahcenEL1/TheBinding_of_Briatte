CC=gcc
CFLAGS=-Wall -g3
LDFLAGS=-lncurses
LDLIBS=
PROGNAME=tbob
ROOTDIR=

all: $(PROGNAME)

crud_piece.o: crud_piece.c crud_piece.h rtbob.h
	$(CC) $(CFLAGS) -c -o $@ $<

rtbob.o: rtbob.c rtbob.h 
	$(CC) $(CFLAGS) -c -o $@ $<

itbob.o: itbob.c itbob.h 
	$(CC) $(CFLAGS) -c -o $@ $<

mtbob.o: mtbob.c mtbob.h 
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.c rtbob.h itbob.h mtbob.h crud_piece.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(PROGNAME): rtbob.o itbob.o mtbob.o crud_piece.o main.o 
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(ROOTDIR)*.o

mrproper: clean
	rm -f $(PROGNAME)
