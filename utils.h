#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define HIGH_BYTE(val) (0xFF & ((val) >> 8))
#define LOW_BYTE(val) (0xFF & (val))

// Uncomment to obfuscate
//#define OBFUSCATE
// Uncomment to change the sentinel byte
#define CHANGE_SEN

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
