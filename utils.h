#ifdef UTILS_H_
#define UTILS_H_
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HIGH_BYTE(val) (0xFF & ((val) >> 8))
#define LOW_BYTE(val) (0xFF & (val))

// Uncomment to obfuscate
//#define OBFUSCATE
// Uncomment to change the sentinel byte
#define CHANGE_SEN

/*

  NOTE: Encryption must be turned on or off in the Makefile because it changes
        which libraries need to be included!

*/
// #define ENCRYPT

#ifdef ENCRYPT
#include <sodium.h>
#include <unistd.h>

typedef struct {
    size_t msg_len; /**< Message length. */
    size_t payload_len; /**< Length of the final payload (excluding this field).*/
    unsigned char *ciphertext; /**< Ciphertext of encrypted message. */
    unsigned char nonce[crypto_secretbox_NONCEBYTES]; /**< Nonce generated for the message. */
    unsigned char salt[crypto_pwhash_SALTBYTES]; /**< Salt generated for the password. */
} encrypted_payload_t;

typedef struct {
    unsigned char key[crypto_secretbox_KEYBYTES];
    unsigned char salt[crypto_pwhash_SALTBYTES];
} key_salt_pair_t;

#endif

/**
 * @brief Struct to hold a message and its size.
 */
typedef struct {
    size_t size; /**< Size of the message in the struct */
    char *msg; /**< Pointer to the actual message value */
} msg_data_t;


/** @brief Function to check file extensions.
  * 
  * This function tests the extenstion on a given filename against a given
  * extension to see if they match. Note: The extension to compare must begin
  * with a dot (.).
  * 
  * @param filename Full filename containing the extension to check.
  * @param desired_extension Extension to compare with. Must start with a dot (ex. ".jpeg").
  * 
  * @return True if the file's extension matches the reference extension, false otherwise.
  */ 
bool test_extension(const char *filename, const char *desired_extension);

/** @brief Reads the message from a text file.
  * 
  * Reads in a message from a text file and puts it in a msg_data_t struct.
  * 
  * @param filename Name of the file to read the message from.
  * 
  * @return msg_data_t struct containing the message. The size field will be negative on error.
  */ 
msg_data_t read_message_from_file(const char *filename);

/** @brief Prints a buffer's value as hex to stdout.
  *
  * @param buf Pointer to the buffer to print.
  * @param size Size of the buffer to print.
  */ 
void print_buf_to_hex(void *buf, size_t size);

#ifdef OBFUSCATE
/** @brief Obfuscate a given message
  *
  * Given a message, it is treated as a bytestring of size @c sz.
  * 
  * It is guaranteed to be safe to have both the destination and the
  * source be the same buffer.
  *
  * @param src The message source buffer
  * @param dst The destination buffer
  * @param sz The number of bytes to obfuscate. A value of 0 means nothing is done.
  */
void obfuscate(const unsigned char *src, unsigned char *dst, size_t sz);
#endif

#ifdef ENCRYPT
/** @brief Generates a key from a known salt and user-supplied password.
  *
  * Using a program-supplied salt, this function generates a key of length
  * `crypto_secretbox_KEYBYTES` to be used in encryption. The key will be placed
  * in a buffer supplied as a parameter.
  * 
  * NOTE: This function provides no memory security management of the passed in 
  * buffer. That should be done by the caller.
  * 
  * NOTE: On error, this function will set the key to 0.
  * 
  * @param key A buffer of length `crypto_secretbox_KEYBYTES` in which to put the
  *            generated key. On the value of the key will be set to 0.
  * @param salt A salt of length `crypto_pwhash_SALTBYTES` to use for key generation.
  */ 
void generate_key(unsigned char *key, unsigned char *salt);

/** @brief Generates a key and salt from a user-supplied password.
  *
  * This function non-determinstically generates a salt and calls the `generate_key`
  * function to use that salt to generate a key from a user supplied password.
  *  
  * NOTE: Calling this function and providing the same password will NOT necessarily
  * generate the same salt value.
  * 
  * NOTE: This function does not do secure memory management of the key. That is
  * the responsibility of the caller.
  * 
  * @param key_salt_pair Pointer to a struct to write the key_salt_pair. On error,
  *                      the function will clear, free, and set this pointer to 0.
  */ 
void generate_key_and_salt(key_salt_pair_t *key_salt_pair);

/** @brief Encrypts a provided message using the provided key and salt pair.
  *
  * This function uses a Libsodium SecretBox to encrypt messages. It takes in a
  * key and salt for the encryption and will generate its own nonce to be used.
  * 
  * NOTE: This function will erase the plaintext!
  * 
  * NOTE: This function does not do secure memory management of the key. That is
  * the responsibility of the caller.
  * 
  * @param plaintext Pointer to the message to encrypt. NOTE: This function will erase the plaintext!
  * @param key_and_salt Pointer to key and salt pair to use for encrypting the message.
  * 
  * @return An encrypted payload containing the newly encrypted message and
  *         associated metadata necessary for decryption.
  */ 
encrypted_payload_t steg_encrypt(msg_data_t *plaintext, key_salt_pair_t *key_and_salt);

/** @brief Decrypts a message recovered from an image file.
  *
  * This function takes in a raw message recovered from an image file, parses it
  * into an encrypted payload, collects a password from the user, and uses that
  * information to retrieve the plaintext message. NOTE: This function performs
  * no checking of the validity of the password.
  * 
  * @param ciphertext_payload The ciphertext message and associated metadata
  *                           necessary to decrypt the message.
  * 
  * @return The plaintext message.
  */
msg_data_t steg_decrypt (encrypted_payload_t *ciphertext_payload);

/** @brief Converts an encrypted payload into a char * buffer.
  *
  * This function takes in an `encrypted_payload_t` struct and packages it into
  * a character buffer of size `payload_len` from the passed in struct. It packs
  * all fields except the payload length into the struct like so: {nonce, salt, 
  * plaintext message length, encrypted message}. Returns NULL if it cannot allocate
  * a buffer to hold the struct. NOTE: Buffer may contain null bytes.
  * 
  * @param payload The encrypted payload to package.
  * 
  * @return A buffer of length `payload->payload_len` containing the payload's
  *         data arranged as follows: {nonce, salt, plaintext message length, 
  *         encrypted message}.
  */ 
char *package_payload (encrypted_payload_t *payload);

/** @brief Extracts a buffer to an `encrypted_payload_t` struct.
  *
  * This function will take a payload encoded by the `package_payload` function
  * and unpack it into an `encrypted_payload_t` struct.
  * 
  * @param raw_msg The raw data to unpack. The `msg` field should be the data
  *                recovered from the image, and the `size` field should be the
  *                data's total size.
  * 
  * @return The unpacked payload.
  */ 
encrypted_payload_t extract_payload (msg_data_t *raw_msg);

#endif
#endif
