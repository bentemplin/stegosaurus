# Stegosaurus
Steganography tool to insert messages into image files. Developed solely for fun; I wouldn't trust that anything in here is bulletproof.

## Dependencies

If encryption is used, Libsodium must be installed. On macOS, it can be installed via HomeBrew; on Windows, it should be downloaded from the libsodium website.

Libsodium can be installed from HomeBrew using the following command:
`brew install libsodium`

## File Descriptions
`main.c`: Runs the program and handles the command line interface. Included functions: `main`.

`utils.c`: Contains helper functions unrelated to hiding and extracting messages from images. Included types: `msg_data_t`, `encrypted_payload_t`, and `key_salt_pair_t`. Included functions: `test_extensions`, `read_msg_from_file`, `print_buf_to_hex`, `obfuscate`, `generate_key`, `generate_key_and_salt`, `steg_encrypt`, `steg_decrypt`, `package_payload` and `extract_payload`.

`stegosaurus.c`: Includes the functions for actually perfoming the steganography. Included functions: `insert_msg_into_file` and `extract_msg_from_img`.

`test_img.jpeg`: Test image that can be used to demonstrate the program. Approximately 7.3 KB in size.

`test_msg.txt`: Test message that can be used to demonstrate the program.

## Background Description
Stegosaurus uses steganography, the practice of hiding something inside something else, to hide messages within a JPEG image.

Look [here](https://en.wikipedia.org/wiki/JPEG#Syntax_and_structure) for info about the JPEG syntax.

## How to Use
This section contains instructions for building, running, and using the program.

### Makefile Options / Conditional Compilation Features
By default, the Makefile has obfuscation and encryption turned on. To turn off
encryption, simply comment out the line `ENCRYPT=1` in the Makefile. To turn off
obfuscation, simply remove the flag `-DOBFUSCATE` from the `CFLAGS` variable.

Both of these features are tagged with conditional compilation, so turning them
off in the Makefile will make sure that their corresponding code is not included
in the compiled program.

### Building
The included Makefile provides the following commands, which should be preceded by `make`.

|    Option     |                  Description                  |
|:-------------:|-----------------------------------------------|
|\[`stegosaurus`\]| Builds the program with obfuscation on.     |
|     `clean`     | Removes compiled files and directories.     |
|  `stego_debug`  | Runs `stegosaurus` target with debug flags. |
|     `help`      | Outputs possible make options.              |


### Running
Use `./stegosaurus` to run the progam. The following table explains which flags are needed and what they do.

| Flag | Required? | Argument Needed? |                                    Description                                     |
|:----:|:---------:|:----------------:|------------------------------------------------------------------------------------|
|  i   |     Y*    |        Y         | Source file for the image. Must have `.jpg` or `.jpeg` extension.                  |
|  m   |     Y*    |        Y         | Source file containing the message to hide.                                        |
|  o   |     Y*    |        Y         | Output file to write the final image. Must have `.jpg` or `.jpeg` extension.       |
|  e   |     Y+    |        Y         | Extract a message from the passed in image. Must have `.jpg` or `.jpeg` extension. |
|  v   |     N     |        N         | Turn on verbose output mode.                                                       |

Arguments followed by * must all be used together to hide a message. Arguments followed by + must be used together to extract a message. Essentially, there are two options to run stegosaurus as follows:
`./stegosaurus -i img_src_file_name -m msg_src_file_name -o img_output_file_name [-v]`
or `./stegosaurus -e img_containing_msg_file_name [-v]`.