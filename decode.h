#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include "types.h"

typedef struct _DecodeInfo
{
	char *stego_image_fname;
	FILE *fptr_stego_image;
	char *output_file_name;
	FILE *fptr_output_file;
} DecodeInfo;

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

OperationType check_operation_type(char *argv[]);

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(char *argv[], DecodeInfo *decInfo);

Status open_file(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *size);

Status decode_secret_file_extn(DecodeInfo *decInfo, int size, char *extn);

Status decode_secret_file_size(DecodeInfo *decInfo, int *size);

Status decode_secret_file_data(DecodeInfo *decInfo, int size);

Status decode_char_from_lsb(char *arr, char *result);

Status decode_int_from_lsb(char *arr, int *result);

Status decode_open_files(DecodeInfo *decInfo);

#endif
