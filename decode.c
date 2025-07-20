#include "types.h"
#include "common.h"
#include "decode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	 if (argv[2] == NULL)
    	 {
        	printf("Filename is required to decode\n");
        	return e_failure;
    	 }
	 if(strstr(argv[2],".bmp")!=NULL)
	 {          
		 decInfo->stego_image_fname = argv[2];
		 return e_success;
	 }
	 else	                                                                                   
	 	return e_failure;
}


Status do_decoding(char *argv[], DecodeInfo *decInfo)
{
	if(decode_open_files(decInfo) == e_success)
	{
		printf("File opened successfully\n");
		fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
		if(decode_magic_string(decInfo) == e_success)
		{
			printf("Magic string decoded successfully\n");
			int size;
			if(decode_secret_file_extn_size(decInfo, &size) == e_success)
			{
				printf("Secret file extension size decoded successfully\n");
				char extn[size];
				if(decode_secret_file_extn(decInfo, size, extn) == e_success)
				{
					printf("Secret file extension decoded successfully\n");
					if(argv[3] != NULL)
					{
						decInfo->output_file_name =  malloc(strlen(argv[3])+strlen(extn)+1);
						strcpy(decInfo->output_file_name,argv[3]);
						strcat(decInfo->output_file_name,extn);
					}
					else
					{
						const char *str = "output";
						decInfo->output_file_name = malloc(strlen(str) + strlen(extn)+1);
						strcpy(decInfo->output_file_name, str);
						strcat(decInfo->output_file_name, extn);
					}
					decInfo->fptr_output_file = fopen(decInfo->output_file_name,"w");

					if (decInfo->fptr_output_file == NULL)
    					{
        					printf("Error opening file.\n");
    					}
					int sec_file_size;
					if(decode_secret_file_size(decInfo, &sec_file_size) == e_success)
					{
						printf("Secret file size decoded successfully\n");
						if(decode_secret_file_data(decInfo, sec_file_size) == e_success)
						{
							printf("Secret file data decoded successfully\n");
						}
						else
						{
							printf("Failed to decode secret file data\n");
						}
					}
					else
					{
						printf("Failed to decode secret file size\n");
					}
				}
				else
				{
					printf("Failed to decode secret file extension\n");
				}
			}
			else
			{
				printf("Failed to decode secret file extension size\n");
			}
		}
		else
		{
			printf("Failed to decode Magic String\n");
		}

	}
	else
	{
		printf("Unable to open file\n");
	}
}

Status decode_open_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
	char magic_string[3];
	for(int i=0; i<2; i++)
	{	
		char ch=0;
		char arr[8];
		fread(arr, 8, 1, decInfo->fptr_stego_image);
		decode_char_from_lsb(arr,&ch);
		magic_string[i]=ch;
	}
	magic_string[2]='\0';
	char str[2];
	printf("Enter the magic string : ");
	scanf(" %s",str);
	if(strcmp(magic_string,str)==0)
		return e_success;
	else
		return e_failure;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *size)
{
	char arr[32];
	fread(arr, 32, 1, decInfo->fptr_stego_image);
	decode_int_from_lsb(arr, size);
	return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo, int size, char *extn)
{
	for(int i=0; i<size; i++)
	{
		char ch=0;
		char arr[8];
		fread(arr, 8, 1, decInfo->fptr_stego_image);
		decode_char_from_lsb(arr,&ch);
		extn[i]=ch;
	}
	extn[size]='\0';
	return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo, int *size)
{
	char arr[32];
	fread(arr, 32, 1, decInfo->fptr_stego_image);
	decode_int_from_lsb(arr,size);
	return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo, int size)
{
	for(int i=0; i<size; i++)
	{
		char ch;
		char arr[8];
		fread(arr, 8, 1, decInfo->fptr_stego_image);
		decode_char_from_lsb(arr,&ch);
		fwrite(&ch, 1, 1, decInfo->fptr_output_file);
	}
	fclose(decInfo->fptr_output_file);
	return e_success;
}

Status decode_char_from_lsb(char *arr, char *result)
{
	char ch=0;
	for(int i=0; i<8; i++)
	{
		ch = (arr[i]&1) << (7-i) | ch;
	}
	*result = ch;

}

Status decode_int_from_lsb(char *arr, int *result)
{
	int num=0;
	for(int i=0; i<32; i++)
	{
		num = (arr[i]&1) << (31-i) | num;
	}	
	*result = num;
}
