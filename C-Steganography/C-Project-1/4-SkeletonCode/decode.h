#ifndef DECODE_H
#define DECODE_H

#include "types.h"// Contains user defined types


#define MAX_MAGIC 100
#define MAX_EXTN 10
#define MAX_SECRET_DATA 2000 

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char *dsecret_fname;      
    FILE *fptr_dsecret;   
    
    int magic_string_size;
    char magic_string[MAX_MAGIC];
    int extn_size;
    char extn[MAX_EXTN];
    char dsecret_data[MAX_SECRET_DATA];    
    int size_dsecret_file;   

} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the Decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status dec_open_files(DecodeInfo *decInfo);

Status skiping_bmp_header(DecodeInfo *decInfo);

Status decode_magic_string_size(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
Status decode_lsb_to_byte(char *data, char *image_buffer);

// Decode a size to lsb
Status decode_lsb_to_size(int *size, char *imageBuffer);

#endif