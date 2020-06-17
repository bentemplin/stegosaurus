#define STEG_SUCCESS_CODE 0
#define STEG_UNSPECIFIED_ERROR -1
#define STEG_FILE_NOT_FOUND -2

#define STEG_MAGIC 0xDEAD
#define JPEG_EOI 0xFFD9
#define HIGH_BYTE(val) (0xFF & ((val) >> 8))
#define LOW_BYTE(val) (0xFF & (val))

// Uncomment to obfuscate
#define OBFUSCATE
// Uncomment to change the sentinel byte
#define CHANGE_SEN


char encrypt_algorithm (char pixel1, char pixel2, char msg_char);
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
int insert_msg_into_file (msg_data_t *msg, char *in_file, char *out_file);

msg_data_t extract_msg_from_file(char *file_name);
