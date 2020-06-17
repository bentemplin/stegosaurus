#define STEG_SUCCESS_CODE 0
#define STEG_UNSPECIFIED_ERROR -1
#define STEG_FILE_NOT_FOUND -2

#define STEG_MAGIC 0xDEAD
#define JPEG_EOI 0xFFD9
#define HIGH_BYTE(val) (0xFF & ((val) >> 8))
#define LOW_BYTE(val) (0xFF & (val))


char encrypt_algorithm (char pixel1, char pixel2, char msg_char);

int insert_msg_into_file (msg_data_t *msg, char *in_file, char *out_file);

msg_data_t extract_msg_from_file(char *file_name);
