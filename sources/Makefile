CLASS=GUI
CC=gcc
CFLAGS=-Wall -g3 -D_$(CLASS)
PROGNAME=tbob_$(CLASS)
ROOTDIR=
LDFLAGS=
LDLIBS=
DEPS=

ifeq ($(CLASS), CLI)
LDFLAGS=-lncurses
DEPS=rtbob.o itbob.o mtbob.o crud_piece.o stage.o tbob_logic.o tbob_cli.o main.c
else
LDFLAGS=-lSDL2 -lSDL2_image
DEPS=rtbob.o itbob.o mtbob.o crud_piece.o stage.o tbob_logic.o tbob_gui.o main.c 
endif

all: $(PROGNAME)

crud_piece.o: crud_piece.c crud_piece.h rtbob.h
	$(CC) $(CFLAGS) -c -o $@ $<

rtbob.o: rtbob.c rtbob.h 
	$(CC) $(CFLAGS) -c -o $@ $<

itbob.o: itbob.c itbob.h 
	$(CC) $(CFLAGS) -c -o $@ $<

mtbob.o: mtbob.c mtbob.h 
	$(CC) $(CFLAGS) -c -o $@ $<

stage.o: stage.c stage.h rtbob.h itbob.h mtbob.h
	$(CC) $(CFLAGS) -c -o $@ $<

tbob_logic.o: tbob_logic.c tbob_logic.h
	$(CC) $(CFLAGS) -c -o $@ $<

tbob_cli.o: tbob_cli.c tbob_cli.h stage.h
	$(CC) $(CFLAGS) -c -o $@ $<

tbob_gui.o: tbob_gui.c tbob_gui.h stage.h
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.c rtbob.h itbob.h mtbob.h stage.h crud_piece.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(PROGNAME): $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(ROOTDIR)*.o

mrproper: clean
	rm -f $(PROGNAME)
