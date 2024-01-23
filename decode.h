#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

//#define MAX_SECRET_BUF_SIZE 4
//#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
//#define MAX_FILE_SUFFIX 4

typedef struct _decodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    int secret_extn_capacity;
    int secret_file_capacity;
    int data;

    /* Output  File Info */
    char *output_fname;
    FILE *fptr_output_image;
    char extn_output_file[33];
    long size_output_file;

} DecodeInfo;


/* Encoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* get file poimters for i/p and o/p files */
Status open_decoding_files(DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Magic string size */
Status decode_magic_string_size(DecodeInfo *decInfo);

/* decode bytes from LSB to get magic string size */
Status decode_lsb_to_int(char *buff,int *data); 

/* decode magic string */
Status decode_magic_string(DecodeInfo *decInfo);

/* decode bytes from LSB to get magic string */
Status decode_lsb_to_char(char *buff,char *data);

/*decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/*decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/*decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif
