# Stegosaurus
Steganography tool to insert messages into image files. Developed solely for fun; I wouldn't trust that anything in here is bulletproof.

## File Descriptions
- `main.c`: contains the main function of the program.
- `io.c`: contains functions to facilitate reading/writing of files for the program.
- `stegosaurus.c`: functions for the steganography, encryption, and command line interface aspects of the program.

## Background Description
Stegosaurus uses steganography, the practice of hiding something inside something else, to hide messages within a JPEG image.

Look [here](https://en.wikipedia.org/wiki/JPEG#Syntax_and_structure) for info about the JPEG syntax.

## How to Use
### Building
- run `make` to compile
### Running
Use `./stegosaurus` to run the progam. Here's what the flags do:
- `-i` \[mandatory\] Source file for your image. Must be `.jpg` or `.jpeg`
- `-m` \[mandatory\] Source file for your message to hide.
- `-o` \[mandatory\] Output file for the program. Must be `.jpg` or `.jpeg`