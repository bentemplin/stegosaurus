# Stegosaurus
Steganography tool to insert messages into image files. Developed solely for fun; I wouldn't trust that anything in here is bulletproof.

## File Descriptions
`main.c`: Runs the program and handles the command line interface. Included functions: `main`.

`utils.c`: Contains helper functions unrelated to hiding and extracting messages from images. Included types: `msg_data_t`. Included functions: `test_extensions`, `read_msg_from_file`, and `obfuscate`.

`stegosaurus.c`: Includes the functions for actually perfoming the steganography. Included functions: `insert_msg_into_file` and `extract_msg_from_img`.

`test_img.jpeg`: Test image that can be used to demonstrate the program. Approximately 7.3 KB in size.

`test_msg.txt`: Test message that can be used to demonstrate the program.

## Background Description
Stegosaurus uses steganography, the practice of hiding something inside something else, to hide messages within a JPEG image.

Look [here](https://en.wikipedia.org/wiki/JPEG#Syntax_and_structure) for info about the JPEG syntax.

## How to Use
This section contains instructions for building, running, and using the program.

### Building
The included makefile provides the following commands, which should be preceded by `make`.

|    Option     |               Description               |
|:-------------:|-----------------------------------------|
| \[`default`\] | Builds the program with obfuscation on. |
|    `clean`    | Removes compiled files and directories. |
|    `debug`    | Runs `default` target with debug flags. |
|    `help`     | Outputs possible make options.          |


### Running
Use `./stegosaurus` to run the progam. The following table explains which flags are needed and what they do.

| Flag | Required? | Argument Needed? |                                 Description                                  |
|:----:|:---------:|:----------------:|------------------------------------------------------------------------------|
|  i   |     Y     |        Y         | Source file for the image. Must have `.jpg` or `.jpeg` extension.            |
|  m   |     Y     |        Y         | Source file containing the message to hide.                                  |
|  o   |     Y     |        Y         | Output file to write the final image. Must have `.jpg` or `.jpeg` extension. |