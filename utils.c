#include "utils.h"

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
    ret.msg = 0;

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

void print_buf_to_hex(void *buf, size_t size) {
    char *char_buf = (char *)buf;
    for (size_t i = 0; i < size; i++) {
        printf("%02x ", 0xff & char_buf[i]);
    }
}

#ifdef OBFUSCATE
void obfuscate(const unsigned char *src, unsigned char *dst, size_t sz) {
    if (sz == 0) return;

    unsigned char reg = src[0];
    dst[0] = reg;
    for (int i = 1; i < sz; ++i) {
        unsigned char c = src[i];
#ifdef CHANGE_SEN
        // 9 == number of sentinel states + 1; we don't XOR the sentinel byte itself!
        if ((i % 9) == 0) {
            reg = c;
            c = 0x0; // Ensures that when we XOR we get original byte back
        }
#endif
        dst[i] = c ^ reg;
        reg = (reg >> 1) + ((reg & 0x1) << 7);
    }
}
#endif

#ifdef ENCRYPT
    void generate_key(unsigned char *key, unsigned char *salt) {

        size_t pass_len_secure;
        char *password_secure = getpass("Enter Password: ");

        if (!password_secure) {
            fprintf(stderr, "Could not retrieve input password! Aborting\n");

            sodium_memzero(key, crypto_secretbox_KEYBYTES);
            return;
        }

        sodium_mlock(password_secure, (pass_len_secure = strlen(password_secure))); // lock the password
        sodium_mlock(&pass_len_secure, sizeof(size_t)); // lock the password's length

        if (crypto_pwhash(key, crypto_secretbox_KEYBYTES, password_secure, 
            strlen(password_secure), salt, crypto_pwhash_OPSLIMIT_INTERACTIVE, 
            crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT) != 0) {
            // error
            fprintf(stderr, "Could not generate key! Aborting\n");

            sodium_munlock(password_secure, pass_len_secure);
            sodium_munlock(&pass_len_secure, sizeof(size_t));

            sodium_memzero(key, crypto_secretbox_KEYBYTES);
            return;
        }
        
        printf("Key generation successful.\n");

        sodium_munlock(password_secure, pass_len_secure);
        sodium_munlock(&pass_len_secure, sizeof(size_t));

    }

    void generate_key_and_salt(key_salt_pair_t *key_salt_pair) {

        randombytes_buf(key_salt_pair->salt, crypto_pwhash_SALTBYTES);

        printf("Salt generation successful.\n");

        generate_key(key_salt_pair->key, key_salt_pair->salt);

        if (!(*key_salt_pair->key)) {
            // on failure, return a zeroed out struct
            sodium_memzero(key_salt_pair, sizeof(key_salt_pair_t));
            key_salt_pair = 0;
            return;
        }
    }

    encrypted_payload_t steg_encrypt(msg_data_t *plaintext, key_salt_pair_t *key_and_salt) {

        encrypted_payload_t message;

        // populate the nonce field
        randombytes_buf(message.nonce, crypto_secretbox_NONCEBYTES);

        // populate the salt field
        for (int i = 0; i < crypto_pwhash_SALTBYTES; i++) {
            message.salt[i] = key_and_salt->salt[i];
        }

        message.msg_len = plaintext->size;

        int ciphertext_len = crypto_secretbox_MACBYTES + message.msg_len;

        message.ciphertext = calloc(ciphertext_len, 1);

        if (!message.ciphertext) {
            message.msg_len = -1;
            sodium_memzero(plaintext, sizeof(*plaintext));
            fprintf(stderr, "Couldn't allocate space for ciphertext! Aborting.\n");
            return message;
        }

        message.payload_len = crypto_secretbox_NONCEBYTES + crypto_pwhash_SALTBYTES + sizeof(message.msg_len) + ciphertext_len;

        // actually do the encryption
        crypto_secretbox_easy(message.ciphertext, (unsigned char *)plaintext->msg, 
            plaintext->size, message.nonce, key_and_salt->key);

        sodium_memzero(plaintext->msg, plaintext->size);
        free(plaintext->msg);
        sodium_memzero(plaintext, sizeof(*plaintext));

        return message;
    }

    msg_data_t steg_decrypt (encrypted_payload_t *ciphertext_payload) {
        msg_data_t ret;
        ret.msg = 0;
        ret.size = 0;

        char *plaintext = calloc(ciphertext_payload->msg_len + 1, sizeof(char));
        if (!plaintext) {
            fprintf(stderr, "Could not allocate space for plaintext message! Aborting.\n");
            
            ret.size = -1;

            return ret;
        }

        unsigned char *key_secure = calloc(crypto_secretbox_KEYBYTES, 1);

        if (!key_secure) {
            fprintf(stderr, "Could not allocate space for decryption key! Aborting.\n");
            
            free(plaintext);
            ret.size = -1;

            return ret;
        }
        
        sodium_mlock(key_secure, crypto_secretbox_KEYBYTES);
        generate_key(key_secure, ciphertext_payload->salt);

        if (*key_secure == 0) {
            sodium_munlock(key_secure, crypto_secretbox_KEYBYTES);
            free(key_secure);
            free(plaintext);

            ret.size = -1;
            return ret;
        }

        if (crypto_secretbox_open_easy((unsigned char *)plaintext, 
            ciphertext_payload->ciphertext, ciphertext_payload->msg_len 
            + crypto_secretbox_MACBYTES, ciphertext_payload->nonce,
            key_secure) != 0) {

            // decryption failed!
            sodium_munlock(key_secure, crypto_secretbox_KEYBYTES);
            free(key_secure);

            sodium_memzero(plaintext, ciphertext_payload->msg_len + 1);
            free(plaintext);

            fprintf(stderr, "Decryption failed, possibly due to an incorrect password! Aborting.\n");

            ret.size = -1;
            return ret;
        }

        // decryption worked!
        sodium_munlock(key_secure, crypto_secretbox_KEYBYTES);
        free(key_secure);

        ret.size = ciphertext_payload->msg_len;
        ret.msg = plaintext;

        return ret;
    }

    char *package_payload(encrypted_payload_t *payload) {
        
        char *ret_buf = calloc(payload->payload_len, 1);

        printf("Payload length:\t%zu\n", payload->payload_len);

        if (!ret_buf) {
            fprintf(stderr, "Couldn't allocate space to package payload! Aborting.\n");
            return 0;
        }

        memcpy(ret_buf, payload->nonce, crypto_secretbox_NONCEBYTES);
        memcpy(ret_buf + crypto_secretbox_NONCEBYTES, payload->salt, 
            crypto_pwhash_SALTBYTES);
        memcpy(ret_buf + crypto_secretbox_NONCEBYTES + crypto_pwhash_SALTBYTES, 
            &(payload->msg_len), sizeof(payload->msg_len));
        memcpy(ret_buf + crypto_secretbox_NONCEBYTES + crypto_pwhash_SALTBYTES + 
            sizeof(payload->msg_len), payload->ciphertext, 
            payload->msg_len + crypto_secretbox_MACBYTES);

        return ret_buf;
    }

    encrypted_payload_t extract_payload (msg_data_t *raw_msg) {
        encrypted_payload_t ret_payload;

        memcpy(ret_payload.nonce, raw_msg->msg, crypto_secretbox_NONCEBYTES);
        memcpy(ret_payload.salt, raw_msg->msg + crypto_secretbox_NONCEBYTES,
            crypto_pwhash_SALTBYTES);
        ret_payload.msg_len = *((size_t *)(raw_msg->msg + crypto_secretbox_NONCEBYTES
            + crypto_pwhash_SALTBYTES));
        ret_payload.ciphertext = (unsigned char *)(raw_msg->msg + 
            crypto_secretbox_NONCEBYTES + crypto_pwhash_SALTBYTES + 
            sizeof(ret_payload.msg_len));

        size_t actual_len = crypto_secretbox_NONCEBYTES + crypto_pwhash_SALTBYTES
            + sizeof(size_t) + ret_payload.msg_len + crypto_secretbox_MACBYTES;

        if (raw_msg->size != actual_len) {

            // passed in payload length didn't match calculated payload length
            fprintf(stderr, "Corrupted message payload! Aborting.\n\tExpected %zu. Got %zu. MSG LEN: %zu", 
                raw_msg->size, actual_len, ret_payload.msg_len);
            ret_payload.msg_len = -1;
            ret_payload.payload_len = -1;
            ret_payload.ciphertext = 0;

            return ret_payload;
        }

        ret_payload.payload_len = raw_msg->size;

        return ret_payload;
    }

#endif
