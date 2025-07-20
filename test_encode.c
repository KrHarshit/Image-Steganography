#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
   //call the function
   int ret = check_operation_type(argv);
   if(ret==e_encode)
   {
    printf("Selected encoding\n");
    EncodeInfo encInfo;
    if(read_and_validate_encode_args(argv,&encInfo) == e_success)
    {
        printf("Reading and validation is done successfully\n");
        do_encoding(&encInfo);
    }
    else
    {
        printf("Failed to read and validate\n");
    }
   }
   else if(ret==e_decode)
   {
   	printf("Selected decoding\n");
	DecodeInfo decInfo;
	if(read_and_validate_decode_args(argv,&decInfo) == e_success)
	{
		printf("Reading and validation is done successfully\n");
		do_decoding(argv,&decInfo);
	}
	else
	{
		printf("Failed to read and validate\n");
	}
   }
   else
   printf("ERROR : \n");
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
    return e_encode;
    else if(strcmp(argv[1],"-d")==0)
    return e_decode;
    else
    return e_unsupported;
}
