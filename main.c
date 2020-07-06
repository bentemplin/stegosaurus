#include <unistd.h>
#include "utils.h"
#include "stegosaurus.h"

int main (int argc, char **argv) {

    char *msg_fname = 0, *img_fname = 0, *out_fname = 0;

    // parse and validate arguments
    static char usage[] = "usage: %s -m message_file_name -i source_img_file -o output_img_file [-v]\n";

    int opt;
    short mflag = 0, iflag = 0, oflag = 0, err = 0, verbose = 0;
    while ((opt = getopt(argc, argv, "m:i:o:v")) != -1) {
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
            case 'v':
                verbose = 1;
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
        return 1;
    }

    if (strcmp(img_fname, out_fname) == 0) {
        fprintf(stderr, "%s: Output file cannot be the same as the input file!\n", argv[0]);
        return 1;
    }

    // do the actual work!
    msg_data_t msg = read_message_from_file(msg_fname);

    if (msg.size == -1) {
        return 1;
    }

    // encrypt if necessary
#ifdef ENCRYPT

    key_salt_pair_t *key_info = calloc(1, sizeof(key_salt_pair_t));

    if (!key_info) {
        fprintf(stderr, "Couldn't allocate key salt pair! Aborting.\n");
        sodium_munlock(msg.msg, msg.size);
        free(msg.msg);

        return 1;
    }

    sodium_mlock(key_info, sizeof(key_salt_pair_t));

    generate_key_and_salt(key_info);

    if (*key_info->key == 0) {
        sodium_munlock(key_info, sizeof(key_salt_pair_t));
        fprintf(stderr, "Couldn't generate key. Aborting\n");
        return 1; // key generation failed
    } else if (verbose) {
        printf("Salt and key generation successful.\n");
    }

    encrypted_payload_t enc_payload = steg_encrypt(&msg, key_info);

    sodium_munlock(key_info, sizeof(key_salt_pair_t));
    free(key_info);
    
    if (verbose) {
        printf("Nonce:\t\t\t");
        print_buf_to_hex(enc_payload.nonce, crypto_secretbox_NONCEBYTES);

        printf("\nSalt:\t\t\t");
        print_buf_to_hex(enc_payload.salt, crypto_pwhash_SALTBYTES);
        printf("\n");

        printf("Message length:\t\t%zu\nEncrypted payload:\t", enc_payload.msg_len);

        print_buf_to_hex(enc_payload.ciphertext, strlen((char *)enc_payload.ciphertext));
        printf("\n");
    }
    
    sodium_munlock(msg.msg, msg.size);
    free(msg.msg);

    msg.msg = package_payload(&enc_payload);

    if (!msg.msg) {
        free(enc_payload.ciphertext);
        fprintf(stderr, "Couldn't alloc space for payload! Aborting.\n");
        return 1;
    } else if (verbose) {
        printf("Payload length:\t%zu\n", enc_payload.payload_len);
    }

    msg.size = enc_payload.payload_len;

    free(enc_payload.ciphertext);

#endif

    // hide the message
    int result = insert_msg_into_file(&msg, img_fname, out_fname);
    free(msg.msg);
    msg.size = -1;

    if (result != STEG_SUCCESS_CODE) return 1;

    // retreive it
    msg_data_t extracted_msg = extract_msg_from_img(out_fname);
    if (extracted_msg.size < 0) {
        // error
        fprintf(stderr, "Could not extract message! Aborting.\n");
        return 1;
    } 
#ifdef OBFUSCATE
    if (verbose) {
        printf("Obfuscated: 0x");
        for (int c = 0; c < extracted_msg.size; ++c) {
            printf("%02x", extracted_msg.msg[c] & 0xff);
        }
        printf("\n");
    }
    obfuscate((unsigned char*)extracted_msg.msg, (unsigned char*)extracted_msg.msg, extracted_msg.size);
#endif
    if (verbose) {
        printf("Extracted Message: ");
        print_buf_to_hex(extracted_msg.msg, extracted_msg.size);
        printf("\n");
    }

#ifdef ENCRYPT
    encrypted_payload_t recovered_payload = extract_payload(&extracted_msg);

    if (recovered_payload.msg_len == -1 || recovered_payload.payload_len == -1) {
        free(extracted_msg.msg);
        return 1;
    }
    if (verbose) {
        printf("Nonce:\t\t\t");
        print_buf_to_hex(recovered_payload.nonce, crypto_secretbox_NONCEBYTES);

        printf("\nSalt:\t\t\t");
        print_buf_to_hex(recovered_payload.salt, crypto_pwhash_SALTBYTES);
        printf("Message length:\t\t%zu\nPayload size:\t\t%zu\nEncrypted message:\t", recovered_payload.msg_len, 
            recovered_payload.payload_len);
        print_buf_to_hex(recovered_payload.ciphertext, recovered_payload.msg_len + crypto_secretbox_MACBYTES);
        printf("\n");
    }

    free(extracted_msg.msg);
    extracted_msg = steg_decrypt(&recovered_payload);

    if (extracted_msg.size == -1) {
        free(extracted_msg.msg);
        return 1;
    }

#endif
    if (verbose) {
        printf("Final Message:\t%s\n", extracted_msg.msg);
        printf("Message size:\t%zu\n", extracted_msg.size);
    } else {
        printf("%s\n", extracted_msg.msg);
    }
#ifdef ENCRYPT
    sodium_memzero(extracted_msg.msg, extracted_msg.size); // if the plaintext was encrypted, erase it.
#endif
    free(extracted_msg.msg);

    return 0;
}
