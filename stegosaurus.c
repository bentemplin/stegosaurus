#include "utils.h"
#include "stegosaurus.h"

// char encrypt_algorithm (char pixel1, char pixel2, char msg_char) {
//     char xor_1, xor_2;

//     xor_1 = pixel1 ^ pixel2;
//     xor_2 = xor_1 ^ msg_char;
//     return pixel1 ^ xor_2;
//     // = pixel1 ^ xor_1 ^ msg_char
//     // = pixel1 ^ pixel1 ^ pixel2 ^ msg_char
// }

int insert_msg_into_file (msg_data_t *msg, char *in_file, char *out_file) {
    // make sure we didn't get any null pointers
    if (!msg || !in_file || !out_file) return STEG_UNSPECIFIED_ERROR;

#ifdef OBFUSCATE
    obfuscate((unsigned char*)msg->msg, (unsigned char*)msg->msg, msg->size);
#endif
    FILE *in_img = fopen(in_file, "rb");

    if (!in_img) {
        fprintf(stderr, "Couldn't open input file named %s.", in_file);
        return STEG_FILE_NOT_FOUND;
    }

    FILE *out_img = fopen(out_file, "wb");

    if (!out_file) {
        fprintf(stderr, "Couldn't open output file named %s.", out_file);
        fclose(in_img);
        return STEG_FILE_NOT_FOUND;
    }

    char cur_byte = 0;
    bool found_eoi1 = 0;
    bool injected_msg = 0;

    while (fread(&cur_byte, sizeof(char), 1, in_img) > 0) {
        fwrite(&cur_byte, sizeof(char), 1, out_img);
        // printf("CUR BYTE: %02x | FOUND EOI1 %x\n", cur_byte, found_eoi1);
        if (found_eoi1 && ((unsigned short)cur_byte & 0xFF) == LOW_BYTE(JPEG_EOI) && !injected_msg) {
            // found end of the image, write the message and another d9
            int eoi_high = HIGH_BYTE(JPEG_EOI);
            int eoi_low = LOW_BYTE(JPEG_EOI);
            int steg_magic_high = HIGH_BYTE(STEG_MAGIC);
            int steg_magic_low = LOW_BYTE(STEG_MAGIC);
            injected_msg = true;
            fwrite(&steg_magic_high, sizeof(char), 1, out_img);
            fwrite(&steg_magic_low, sizeof(char), 1, out_img);
            fwrite(msg->msg, sizeof(char), msg->size, out_img);
            fwrite(&steg_magic_high, sizeof(char), 1, out_img);
            fwrite(&steg_magic_low, sizeof(char), 1, out_img);
            fwrite(&eoi_high, sizeof(char), 1, out_img);
            fwrite(&eoi_low, sizeof(char), 1, out_img);
        }
        found_eoi1 = ((unsigned short)cur_byte & 0xFF) == HIGH_BYTE(JPEG_EOI);
    }

    fclose(in_img);
    fclose(out_img);
    return STEG_SUCCESS_CODE;
}

msg_data_t extract_msg_from_img(char *file_name) {
    msg_data_t ret;
    ret.size = STEG_UNSPECIFIED_ERROR;
    ret.msg = 0;

    if (!file_name) return ret;

    FILE *file = fopen(file_name, "rb");

    if (!file) {
        ret.size = STEG_FILE_NOT_FOUND;
        return ret;
    }

    char cur_byte = 0;
    bool found_magic_1 = 0;
    bool in_msg = 0;
    int msg_size = 0;

    // find size of message
    while(fread(&cur_byte, sizeof(char), 1, file) > 0) {
        if (in_msg) {
            msg_size++;
        }
        if (found_magic_1 && (((unsigned short)cur_byte & 0xFF) == LOW_BYTE(STEG_MAGIC))) {
            if (in_msg) {
                msg_size -= 2;
                break;
            }
            else (in_msg) = true;
        }
        found_magic_1 = ((unsigned short)cur_byte & 0xFF) == HIGH_BYTE(STEG_MAGIC);
    }

    fseek(file, -(msg_size + 2), SEEK_CUR);
    char *msg_buf = malloc(msg_size);
    if (!msg_buf) return ret;
    
    ret.msg = msg_buf;
    ret.size = msg_size;

    if (fread(msg_buf, sizeof(char), msg_size, file) != msg_size) {
        ret.size = STEG_UNSPECIFIED_ERROR;
        free(ret.msg);
    }
    
    // for (int i = 0; i < msg_size; i++) {
    //     fread(&cur_byte, sizeof(char), 1, file);
    //     printf("\"%c\" %d\n", cur_byte, i);
    // }

    // printf("\nMSG SIZE: %d\n", msg_size);
    return ret;
}
