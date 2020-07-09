# Basic Stuff
CC=gcc
FILES=main.c stegosaurus.c utils.c 
OUT_FILE=stegosaurus

# Flags
CFLAGS=-Wall -Werror -Wpedantic -DOBFUSCATE
ENCRYPT_FLAG=-DENCRYPT
DEBUG_FLAG=-g -DDEBUG

# Libs
ENCRYPT_LIB=-lsodium

# Conditional Stuff
# - Comment out a line to turn off that feature
ENCRYPT=1

ifdef ENCRYPT
stegosaurus: $(FILES)
	@ $(CC) $(CFLAGS) $(ENCRYPT_FLAG) $(ENCRYPT_LIB) $(FILES) -o $(OUT_FILE)
stego_debug: $(FILES)
	@ $(CC) $(CFLAGS) $(DEBUG_FLAG) $(ENCRYPT_FLAG) $(ENCRYPT_LIB) $(FILES) -o $(OUT_FILE)
else 
stegosaurus:
	@ $(CC) $(CFLAGS) $(FILES) -o $(OUT_FILE)
stego_debug:
	@ $(CC) $(CFLAGS) $(DEBUG_FLAG) $(FILES) -o $(OUT_FILE)
endif

clean:
	@ rm -f $(OUT_FILE)
	@ rm -f -r *.dSYM


help:
	@ echo
	@ echo "make: build stegosaurus"
	@ echo "make clean: remove compiled stegosaurus files"
	@ echo "make debug: compile with debug flags"
	@ echo
