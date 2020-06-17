#include "io.h"

bool test_extension(const char *filename, const char *desired_extension) {
    size_t fname_len = strlen(filename);
    size_t ext_len = strlen(desired_extension);

    if (ext_len == 0) {
        fprintf(stderr, "Can't match extension of length 0\n");
        return false; // can't match a extension of length 0
    } else if (fname_len == 0) {
        fprintf(stderr, "Can't match filename of length 0\n");
        return false; // can't match a filename of length 0
    }

    size_t fname_idx = fname_len - 1;
    while (fname_idx >= 0 && filename[fname_idx] != '.') fname_idx--; // find last . in filename

    if (fname_idx + ext_len != fname_len) return false; // filename w/o ext + desired ext != length of filename

    size_t ext_idx = 0;
    while (fname_idx < fname_len && ext_idx < ext_len) {
        if (filename[fname_idx++] != desired_extension[ext_idx++]) return false;
    }

    return true;
}

msg_data_t read_message_from_file(const char *filename) {

    msg_data_t ret;
    ret.size = -1;
    ret.msg = "";

    if (!test_extension(filename, ".txt")) {
        fprintf(stderr, "Can't read message from non .txt file! Filename: \"%s\"\n", filename);
        return ret;
    } 

    FILE *msg_fp = fopen(filename, "r");

    if (!msg_fp) {
        // message file not found
        fprintf(stderr, "Couldn't find message file with name \"%s\"!\n", filename);
        return ret;
    }

    // find length of message
    fseek(msg_fp, 0l, SEEK_END);
    size_t msg_len = ftell(msg_fp);
    rewind(msg_fp);

    ret.msg = calloc(msg_len, sizeof(char));
    if (!ret.msg) {
        fprintf(stderr, "Couldn't read in message - malloc failed\n");
        fclose(msg_fp);
        return ret;
    }

    ret.size = msg_len;

    // copy message into ret buffer
    int copy_idx = 0;
    char curr_char;
    while ((curr_char = fgetc(msg_fp)) != EOF) {
        ret.msg[copy_idx++] = curr_char;
    }

    fclose(msg_fp);

    return ret;
}
