CC=gcc
CFLAGS=-Wall -Werror -Wpedantic -DOBFUSCATE
DEBUG_FLAG=-g
FILES=main.c stegosaurus.c utils.c 
OUT_FILE=stegosaurus

default:
	@ $(CC) $(CFLAGS) $(FILES) -o $(OUT_FILE)

clean:
	@ rm -f $(OUT_FILE)
	@ rm -f -r *.dSYM

debug:
	@ $(CC) $(CFLAGS) $(DEBUG_FLAG) $(FILES) -o $(OUT_FILE)

help:
	@ echo
	@ echo "make: build stegosaurus"
	@ echo "make clean: remove compiled stegosaurus files"
	@ echo "make debug: compile with debug flags"
	@ echo

