#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


typedef struct {
    size_t size;
    char *msg;
} msg_data_t;

bool test_extension(const char *filename, const char *desired_extension);

msg_data_t read_message_from_file(const char *filename);
