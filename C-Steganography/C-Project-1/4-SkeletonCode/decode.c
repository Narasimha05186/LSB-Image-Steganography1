#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"


// This function checks command-line arguments and takes stego image + output file
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2] == NULL)
    {
        printf("Invalid input\n");
        return e_failure;
    }
    if(strstr(argv[2],".bmp") != NULL)
    {
        printf(".bmp is present\n");
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf(".bmp is not present\n");
        return e_failure;
    }
    
    if(argv[3] == NULL)
    {
        goto OUTPUT;
    }
    
    if(strstr(argv[3], ".txt") != NULL ) 
    {
        printf("For output .txt file present\n");
        decInfo->dsecret_fname = argv[3];
    }
    else
    {
        OUTPUT:
        decInfo->dsecret_fname = "output.txt";
    }
    return e_success;
}


// This function controls the entire decoding process step by step
Status do_decoding(DecodeInfo *decInfo)
{
    if(dec_open_files(decInfo) == e_success)
    {
        printf("All files are open successfull\n");
    }
    else
    {
        printf("All files are not open successfull\n");
        return e_failure;
    }

    if(skiping_bmp_header(decInfo) == e_success)
        printf("Skiping of bmp header is successfull\n");
    else
        return e_failure;

    if(decode_magic_string_size(decInfo) == e_success)
        printf("Magic string size decoded successfull\n");
    else
        return e_failure;

    if(decode_magic_string(decInfo) == e_success)
        printf("Magic string decode successfull\n");
    else
        return e_failure;

    if(decode_secret_file_extn_size(decInfo) == e_success)
        printf("Decode of secret file extn size is successfull\n");
    else
        return e_failure;

    if(decode_secret_file_extn(decInfo) == e_success)
        printf("Decode of secret file extn is successfull\n");

    if(decode_secret_file_size(decInfo) == e_success)
        printf("Decode secret file size successfull\n");
    else
        return e_failure;

    if(decode_secret_file_data(decInfo) == e_success)
        printf("Decode secret file data is successfull\n");

    if (decInfo->fptr_stego_image)
    {
        fclose(decInfo->fptr_stego_image);
        decInfo->fptr_stego_image = NULL;
    }
    if (decInfo->fptr_dsecret)
    {
        fclose(decInfo->fptr_dsecret);
        decInfo->fptr_dsecret = NULL;
    }

    printf("All files closed successfull\n");
    return e_success;
}


// This function opens the stego image file for reading
Status dec_open_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname,"r");
    if(decInfo->fptr_stego_image == NULL)
    {
        printf("%s file is not present\n",decInfo->stego_image_fname);
        return e_failure;
    }
    return e_success;
}


// This function skips the first 54 bytes (BMP header)
Status skiping_bmp_header(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET); 
    return e_success;
}


// This function reads 32 bits to get the magic string size
Status decode_magic_string_size(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer, 32, 1, decInfo->fptr_stego_image);
    decode_lsb_to_size(&decInfo->magic_string_size,buffer);
    return e_success;
}


// This function extracts a 32-bit number from LSBs of 32 bytes
Status decode_lsb_to_size(int *size, char *imageBuffer)
{
    *size = 0;
    for (int i = 0; i < 32; i++)
    {
        *size |= (imageBuffer[i] & 1) << i;
    }
    return e_success;
}


// This function decodes the magic string and checks it with user input
Status decode_magic_string(DecodeInfo *decInfo) 
{
    char buffer[8];
    
    if (decInfo->magic_string_size < 0 || decInfo->magic_string_size >= (int)sizeof(decInfo->magic_string)) 
    {
        printf("Invalid magic string size: %d\n", decInfo->magic_string_size);
        return e_failure;
    }

    for (int i = 0; i < decInfo->magic_string_size; i++)
    {
        if (fread(buffer, 8, 1, decInfo->fptr_stego_image) != 1) 
        {
            printf("Error: unexpected EOF while reading magic string.\n");
            return e_failure;
        }
        decode_lsb_to_byte(&decInfo->magic_string[i], buffer);
    }
    decInfo->magic_string[decInfo->magic_string_size] = '\0';

    char password[100];
    printf("Enter the magic string: ");
    scanf(" %[^\n]",password);

    if(strcmp(password, decInfo->magic_string) != 0)
        return e_failure;
    
    return e_success;
}


// This function extracts one byte of hidden data from 8 bytes of image
Status decode_lsb_to_byte(char *data, char *image_buffer) 
{
    *data = 0;
    for (int i = 0; i < 8; i++)
    {
        *data |= (image_buffer[i] & 1) << i;
    }
    return e_success;
}


// This function decodes the size of secret file extension
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer, 32, 1, decInfo->fptr_stego_image);
    decode_lsb_to_size(&decInfo->extn_size, buffer);
    return e_success;
}


// This function extracts the secret file extension (like .txt)
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[8];

    printf("Decoded secret file extn: ");
    for (int i = 0; i < decInfo->extn_size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        decode_lsb_to_byte(&decInfo->extn[i], buffer); 
    }
    decInfo->extn[decInfo->extn_size] = '\0';
    printf("%s\n", decInfo->extn);
    
    return e_success;
}


// This function reads 32 bits to get the secret file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer, 32, 1, decInfo->fptr_stego_image);
    decode_lsb_to_size(&decInfo->size_dsecret_file,buffer);
    return e_success;
}


// This function decodes secret file data and writes it to output file
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    if (decInfo->size_dsecret_file < 0 || decInfo->size_dsecret_file >= (int)sizeof(decInfo->dsecret_data)) 
    {
        printf("Error: secret file size invalid or too large (%d).\n", decInfo->size_dsecret_file);
        return e_failure;
    }

    char buffer[8];

    for (int i = 0; i < decInfo->size_dsecret_file; i++)
    {
        if (fread(buffer, 8, 1, decInfo->fptr_stego_image) != 1) 
        {
            printf("Error: unexpected EOF while reading stego image for secret data.\n");
            return e_failure;
        }
        decode_lsb_to_byte(&decInfo->dsecret_data[i], buffer);
    }
    decInfo->dsecret_data[decInfo->size_dsecret_file] = '\0';

    decInfo->fptr_dsecret = fopen(decInfo->dsecret_fname, "w");
    if (decInfo->fptr_dsecret == NULL)
    {
        printf("Error: Unable to open %s for writing.\n", decInfo->dsecret_fname);
        return e_failure;
    }

    if (fwrite(decInfo->dsecret_data, 1, decInfo->size_dsecret_file, decInfo->fptr_dsecret) 
        != (size_t)decInfo->size_dsecret_file) 
    {
        printf("Error: write failed for %s.\n", decInfo->dsecret_fname);
        fclose(decInfo->fptr_dsecret);
        decInfo->fptr_dsecret = NULL;
        return e_failure;
    }

    printf("Decoded secret data successfully written to %s\n", decInfo->dsecret_fname);
    return e_success;
}
