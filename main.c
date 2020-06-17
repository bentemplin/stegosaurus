// #include <getopt.h>
#include <unistd.h>
#include "io.h"
#include "stegosaurus.h"

int main (int argc, char **argv) {

    char *msg_fname = 0, *img_fname = 0, *out_fname = 0;

    // parse and validate arguments
    static char usage[] = "usage: %s -m message_file_name -i source_img_file -o output_img_file\n";

    int opt;
    short mflag = 0, iflag = 0, oflag = 0, err = 0;
    while ((opt = getopt(argc, argv, "m:i:o:")) != -1) {
        switch (opt) {
            case 'm':
                mflag = 1;
                msg_fname = optarg;
                break;
            case 'i':
                img_fname = optarg;
                iflag = (test_extension(img_fname, ".jpg") || test_extension(img_fname, ".jpeg")) ? 1 : -1;
                break;
            case 'o':
                out_fname = optarg;
                oflag = (test_extension(out_fname, ".jpg") || test_extension(out_fname, ".jpeg")) ? 1 : -1;
                break;
            default:
               err = 1;
        }
    }
    if (mflag == 0) {
        fprintf(stderr, "%s: missing -m message file option\n", argv[0]);
        err = 1;
    }
    if (iflag == 0) {
        fprintf(stderr, "%s: missing -i image file option\n", argv[0]);
        err = 1;
    } else if (iflag == -1) {
        fprintf(stderr, "%s: -i image file must be a .jpg or .jpeg\n", argv[0]);
        err = 1;
    }
    if (oflag == 0) {
        fprintf(stderr, "%s: missing -o output file option\n", argv[0]);
        err = 1;
    } else if (oflag == -1) {
        fprintf(stderr, "%s: -o output file must be a .jpg or .jpeg\n", argv[0]);
        err = 1;
    }
    if (err) {
        fprintf(stderr, usage, argv[0]);
        exit(1);
    }

    if (strcmp(img_fname, out_fname) == 0) {
        fprintf(stderr, "%s: Output file cannot be the same as the input file!\n", argv[0]);
        exit(1);
    }

    // do the actual work!
    msg_data_t msg = read_message_from_file(msg_fname);

    // hide the message
    int result = insert_msg_into_file(&msg, img_fname, out_fname);
    if (result != STEG_SUCCESS_CODE) exit(1);

    // retreive it
    msg_data_t extracted_msg = extract_msg_from_file(out_fname);
    if (extracted_msg.size < 0) {
        // error
        free(extracted_msg.msg);
        exit(1);
    } 
#ifdef OBFUSCATE
    printf("Obfuscated: 0x");
    for (int c = 0; c < extracted_msg.size; ++c) {
        printf("%02x", extracted_msg.msg[c] & 0xff);
    }
    printf("\n");
    obfuscate((unsigned char*)extracted_msg.msg, (unsigned char*)extracted_msg.msg, extracted_msg.size);
#endif
    printf("%s\n", extracted_msg.msg);
    free(extracted_msg.msg);

    return 0;
}
