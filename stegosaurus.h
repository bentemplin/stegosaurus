// success code is 0 by convention, error codes are negative so they can be
// returned as sized in msg_data_t structs.

#define STEG_SUCCESS_CODE 0
#define STEG_UNSPECIFIED_ERROR -1
#define STEG_FILE_NOT_FOUND -2

#define STEG_MAGIC 0xDEAD // used to detect a hidden message in a file
#define JPEG_EOI 0xFFD9 // end of image signifier for JPEG format

/** @brief Adds a message into an image file and produces a new one.
  * 
  * Given an image file and a message, this function produces a new image file
  * with the image hidden inside.
  * 
  * @param msg Message data struct to hide.
  * @param in_file Filename of the file to use as the image.
  * @param out_file Filename of the file to write the image and the message.
  * 
  * @return Success code of the operation. See #define statements above for key.
  */
int insert_msg_into_file (msg_data_t *msg, char *in_file, char *out_file);

/**
  * @brief Retrieves a hidden message from an image file.
  * 
  * Retreives a message hidden in an image file. Does not modify or destroy the
  * image file.
  * 
  * @param file_name Filename of the file to use for message extraction.
  * 
  * @return A msg_data_t struct containing the message.
  */
msg_data_t extract_msg_from_img (char *file_name);
