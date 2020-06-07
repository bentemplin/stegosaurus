// #include <getopt.h>
#include <unistd.h>
#include "io.h"
#include "stegosaurus.h"

int main (int argc, char **argv) {
    blank2();

    char *msg_fname = 0;

    // parse and validate arguments
    static char usage[] = "usage: %s -m message_file_name\n";

    int opt;
    short mflag = 0, err = 0;
    while ((opt = getopt(argc, argv, "m:")) != -1) {
        switch (opt) {
            case 'm':
                mflag = 1;
                msg_fname = optarg;
                break;
            default:
               err = 1;
        }
    }
    if (mflag == 0) {
        fprintf(stderr, "%s: missing -m message file option\n", argv[0]);
        fprintf(stderr, usage, argv[0]);
        exit(1);
    } else if (err) {
        fprintf(stderr, usage, argv[0]);
        exit(1);
    }

    // do the actual work!
    msg_data_t msg = read_message_from_file(msg_fname);

    printf("--- BEGIN MESSAGE ---\n\n%s\n\n--- END MESSAGE ---\n", msg.msg);

    return 0;
}
