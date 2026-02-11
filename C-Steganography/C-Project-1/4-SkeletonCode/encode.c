#include <stdio.h>
#include <string.h>
#include "types.h"
#include "encode.h"


// Validates input for Encoding 
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2] == NULL)
    {
        printf("Invalid input\n");
        return e_failure;
    }
    if(strstr(argv[2], ".bmp") != NULL)
    {
        printf(".bmp file is Present\n");
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf(".bmp file is not Present\n");
        return e_failure;
    }

    if(argv[3] == NULL)
    {
        printf("Invalid input\n");
        return e_failure;
    }
    if(strstr(argv[3], ".txt") != NULL)
    {
        printf(".txt file is present\n");
        encInfo->secret_fname = argv[3];
    }
    else
    {
        printf(".txt file is not present\n");
        return e_failure;
    }

    if(argv[4] == NULL)
        goto STEGO;

    if(strstr(argv[4], ".bmp") != NULL)
    {
        printf("stego.bmp is Present\n");
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        STEGO:
        encInfo->stego_image_fname = "stego.bmp";
    }
    return e_success;
}


//Calling Encoding steps
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
        printf("All files are opened\n");
    else
    {
        printf("All files are not open\n");
        return e_failure;
    }

    if(check_capacity(encInfo) == e_success)
        printf("Capacity checking is successfull\n");
    else
    {
        printf("Sorry..!Capacity checking is un-successfull\n");
        return e_failure;
    }

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
        printf("bmp header is copyed successfull\n");
    else
    {
        printf("Sorry...!bmp header copying is un-successfull\n");
        return e_failure;
    }

    if(encode_magic_string_size(encInfo) == e_success)
        printf("Size of magic string encoded successfully\n");
    else
    {
        printf("Sorry...! Size of magic string not encoded successfully\n");
        return e_failure;
    }

    if(encode_magic_string(encInfo)==e_success)
        printf("Magic string encoded successfully\n");
    else
    {
        printf("Sorry...! Magic string not encoded successfully\n");
    }
    
    int size = strlen(strchr(encInfo->secret_fname,'.'));

    if(encode_size_to_lsb(size, encInfo) == e_success)
        printf("Size of extn encoded successfully\n");
    else
    {
        printf("Sorry...! Size of extn not encoded successfully\n");
        return e_failure;
    }

    if(encode_secret_file_extn(strchr(encInfo->secret_fname,'.'),encInfo)==e_success)
        printf("Secret of extn encoded successfully\n");
    else
    {
        printf("Sorry....! Secret of extn not encoded successfully\n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
        printf("Size of secret file encoded successfully\n");
    else
    {
        printf("Sorry...! Size of secret file not encoded successfully\n");
        return e_failure;
    }

    if(encode_secret_file_data(encInfo)==e_success)
        printf("Secret file data is encoded\n");
    else
    {
        printf("Sorry...! Secret file data is not encoded\n");
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
        printf("Remaining data is copied\n");
    else
    {
        printf("Sorry...! Remaining data is not copied\n");
        return e_failure;
    }

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    printf("All files closed successfully\n");
    
    return e_success;
}


//Opens source image, secret file, and output stego image file
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if(encInfo->fptr_src_image == NULL)
    {
        printf("Sorry...! source file is not present\n");
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if(encInfo->fptr_secret == NULL)
    {
        printf("Sorry...! Secret file is not present\n");
        return e_failure;
    }
    
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname,"w");

    return e_success;
}


// Checks if image has enough capacity to hide secret data
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    printf("Enter the magic string : ");
    scanf("%[^\n]",encInfo->magic);

    if(encInfo->image_capacity > (32 + strlen(encInfo->magic) * 8 + 32 + 32 + 32 + encInfo->size_secret_file * 8))
        return e_success;
    else
        return e_failure;
}


//Width and  height Reading of Image from BMP header 
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, 4, 1, fptr_image);
    fread(&height, 4, 1, fptr_image);

    return width * height * 3;
}


//size of secret file
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}


// Copies 54-byte BMP header from source to stego image
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);

    char buffer[54];

    fread(&buffer, 54, 1, fptr_src_image);
    fwrite(&buffer, 54, 1, fptr_dest_image);

    return e_success;
}


// Encodes the size of magic string into image
Status encode_magic_string_size(EncodeInfo *encInfo)
{
    if(encode_size_to_lsb(strlen(encInfo->magic), encInfo) == e_success)
        return e_success;
    else
        return e_failure;
}


// Encodes actual magic string into image
Status encode_magic_string(EncodeInfo *encInfo)
{
    if(encode_data_to_image(encInfo->magic, strlen(encInfo->magic), encInfo)==e_success)
        return e_success;
    else
        return e_failure;
}


// Encodes byte-by-byte data into image LSB
Status encode_data_to_image(char *data, int size,EncodeInfo *encInfo)
{
    for(int i = 0; i < size; i++)
    {
       fread(encInfo->image_data,8,1,encInfo->fptr_src_image);
       encode_byte_to_lsb(data[i], encInfo->image_data);
       fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}


//Encodes one byte into 8 pixels’ LSB bits
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 0; i < 8; i++)
    {
       image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & (1 << i)) >> i);
    }
}


//Stores 32-bit integer (size) into image LSBs
Status encode_size_to_lsb(int size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    for(int i = 0; i < 32; i++)
    {
        buffer[i] = (buffer[i] & 0xFE) | ((size & (1 << i)) >> i);
    }
    fwrite(buffer, 32 , 1, encInfo->fptr_stego_image);

    return e_success;
}


// Encodes file extension (.txt) into image
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    if(encode_data_to_image(file_extn, strlen(file_extn), encInfo)==e_success)
    {
        printf("Encode secret file successfully\n");
        return e_success;
    }
    else
    {
        printf("Encode secret file un-successfull\n");
        return e_failure;
    }

}


//Encodes the size of secret file
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size,encInfo);
    return e_success;
}


//Reads secret file and encodes its content
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);
    fread(buffer,encInfo->size_secret_file,1,encInfo->fptr_secret);
    encode_data_to_image(buffer,encInfo->size_secret_file,encInfo);

    return e_success;
}


//Copies leftover image data after encoding completes
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    while(fread(&ch,1,1,fptr_src))
       fwrite(&ch,1,1,fptr_dest);

    return e_success;
}


  