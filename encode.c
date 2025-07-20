#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    /*check if argv[2] is .bmp file or not*/
    if(argv[2]==NULL)
    {
	    printf("Filename is required to be encoded\n");
	    return e_failure;
    }
    if(strstr(argv[2],".bmp")!=NULL)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    return e_failure;
    /*check argv[3] is .txt file or not*/
    if(argv[3]==NULL)
    {
	    printf("Secret filename required\n");
	    return e_failure;
    }
    if(strstr(argv[3],".txt") != NULL)
    {
        encInfo->secret_fname=argv[3];
        strcpy(encInfo->extn_secret_file,strstr(argv[3],"."));
    }
    else
    return e_failure;
    /*check argv[4] is passed or not*/
    if(argv[4] != NULL)
    {
        if(strstr(argv[4],".bmp") != NULL)
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        return e_failure;
    }
    else
    {
        encInfo->stego_image_fname = "output.bmp";
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)
    {
        printf("Files are opened successfully\n");
        if(check_capacity(encInfo)==e_success)
        {
            printf("Capacity is successfully checked\n");
            if(copy_bmp_header(encInfo -> fptr_src_image, encInfo->fptr_stego_image)==e_success)
            {
                printf("Copy bmp header is successfully done\n");
                if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("Magic string encoded successfully\n");
                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)
                    {
                        printf("Secret file extension size encoded successfully\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
                        {
                            printf("Secret file extension encoded successfully\n");
                            if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
                            {
                                printf("Secret file size is encoded successfully\n");
                                if(encode_secret_file_data(encInfo)==e_success)
                                {
                                    printf("Secret file data is encoded successfully\n");
                                    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("Remaining data successfully copied\n");
                                    }
                                    else
                                    {
                                        printf("Remaining data copy failure");
                                    }
                                }
                                else
                                {
                                    printf("Secret file data encoding failed\n");
                                }
                            }
                            else
                            {
                                printf("Secret file size encoding failed");
                            }
                        }
                        else
                        {
                            printf("Encoding secret file extension failed\n");
                        }
                    } 
                    else
                    {
                        printf("Encoding secret file extension size failed\n");
                    }
                }
                else
                {
                    printf("Encode magic string failure\n");
                }
            }
            else
            {
                printf("copy_bmp_header function failure\n");
            }
        }
        else
        {
            printf("Error: Capacity check is failed\n");
        }
    }
    else{
        printf("Error : open_files function failed");
    }
}

Status check_capacity(EncodeInfo *encInfo)
{
    /*check beautiful.bmp size > header(54)+
    magic_string(16) + extension size(32) + extension(4*8)
    +secret filesize(32) + secretfiledata*8 */
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity > (54 + strlen(MAGIC_STRING) * 8)+
    32 + 32 + 32 + encInfo->size_secret_file * 8)
    {
        return e_success;
    }
    else
    return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    /*read 54 bytes from fptr_src_image
    write 54 bytes in fptr_stego_image*/
    rewind(fptr_src_image);
    char arr[54];
    fread(arr, 54, 1, fptr_src_image);
    fwrite(arr, 54, 1, fptr_stego_image);
    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    //read 8 bytes from beautiful.bmp file
    char arr[8];
    for(int i=0; i<strlen(magic_string); i++)
    {
        fread(arr, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], arr);
        fwrite(arr, 8, 1, encInfo -> fptr_stego_image);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *arr)
{
    //function used to encode only one character
    for(int i=0; i<8; i++)
    {
        arr[i] = (arr[i]& 0xFE) | ((data &(1<<7-i)) >> (7-i));
    }
}

Status encode_size_to_lsb(int data, char *arr)
{
    for(int i=0; i<32; i++)
    {
        arr[i] = (arr[i] & (~1)) | ((data & (1<<31-i)) >> (31-i));
    }
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char arr[32];
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size,arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    char arr[8];
    for(int i=0; i<strlen(file_extn);  i++)
    {
        fread(arr, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], arr);
        fwrite(arr, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char arr[32];
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char secret_data[encInfo->size_secret_file];
    rewind(encInfo -> fptr_secret);
    fread(secret_data, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    char arr[8];
    for(int i=0; i<encInfo->size_secret_file; i++)
    {
    fread(arr, 8, 1, encInfo->fptr_src_image);
    encode_byte_to_lsb(secret_data[i], arr);
    fwrite(arr, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_des)
{
    char ch;
    while(fread(&ch, 1, 1, fptr_src)>0)
    {
        fwrite(&ch, 1, 1, fptr_des);
    }
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}


/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
