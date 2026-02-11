
// ===============================================================
// PROJECT            : Image Steganography (Encoding & Decoding) |
// AUTHOR             : DASARI VICTOR BABU                        |
// ID                 : 25026_196                                 |
// BATCH              :26_C                                       |
// DATE OF SUBMISSION : (19-11-2025)                              |
// ===============================================================
//=================================================================
// --->SDLC of Image Steganography Project                        |
//=================================================================
// 1. Requirement: Hide secret text inside a BMP image and also retrieve it.
// 2. Analysis: Use LSB (Least Significant Bit) method to store bits of text.
// 3. Design: Two modules - Encoding (store text) and Decoding (extract text).
// 4. Implementation: Write encode.c, decode.c, types.h, main.c.
// 5. Testing: Use different BMP images and text files to verify results.
// 6. Deployment: Give executable with -e and -d command line options.


#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

// Main function: selects encode/decode and runs the operation
int main(int argc, char *argv[])
{
    // Check if user selected encoding
    if(check_operation_type(argv) == e_encode)
    {
        printf("You choosed encoding\n");

        EncodeInfo encInfo;

        // Validate encoding arguments
        if(read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Read and validate is successfull\n");

            // Perform encoding
            if(do_encoding(&encInfo) == e_success)
            {
                printf("encoding is successfull\n");
            }
            else
            {
                printf("encoding is un-successfull\n");
            }
        }
        else
        {
            printf("Read and validate is un-successfull\n");
            return e_failure;
        }
    }

    // Check if user selected decoding
    else if(check_operation_type(argv) == e_decode)
    {
        printf("You choosed decoding\n");

        DecodeInfo decInfo;

        // Validate decoding arguments
        if(read_and_validate_decode_args(argv,&decInfo) == e_success)
        {
            printf("Read and validation is successfull\n");

            // Perform decoding
            if(do_decoding(&decInfo) == e_success)
            {
                printf("Decoding is successfull\n");
            }
            else
            {
                printf("Decoding is un-successfull\n");
            }
        }
        else
        {
            printf("Read and validation is un-successfull\n");
            return e_failure;
        }
    }

    // For invalid arguments
    else
    {
        printf("Pass correct arguments\n");
        printf("./a.out -e beautiful.bmp secret.txt --> for encoding\n");
        printf("./a.out -d stego.bmp --> for decoding\n");
    }
    
    return e_success;
}

// Identifies operation type: encode / decode / unsupported
OperationType check_operation_type(char *argv[])
{
    if(argv[1] == NULL)
        return e_unsupported;

    if(strcmp(argv[1], "-e") == 0)
        return e_encode;

    else if(strcmp(argv[1], "-d") == 0)
        return e_decode;

    else
        return e_unsupported;
}
