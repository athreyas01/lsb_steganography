#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <stdlib.h>
#include <unistd.h>

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Stego Image file(i.e encoded file), output file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_decoding_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");                 // Src Image file
    if (decInfo->fptr_stego_image == NULL)                                            // Do Error handling
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
	return e_failure;
    }
    decInfo->fptr_output_image = fopen(decInfo->output_fname, "w");             // Stego Image file
    if (decInfo->fptr_output_image == NULL)                                          // Do Error handling
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);
	return e_failure;
    }
    return e_success;                                                              // No failure return e_success
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)     //Read and validate Encode args from argv 
{
    if(strcmp(strstr(argv[2],"."),".bmp") == 0)
    {
	decInfo->stego_image_fname = argv[2];
	if (argv[3] != NULL)
	    decInfo->output_fname=argv[3];
	else
	    decInfo->output_fname = "out.txt";
	return e_success;
    }
    else
	return e_failure;
}

Status do_decoding(DecodeInfo *decInfo)
{
	if(open_decoding_files(decInfo) == e_success)
	{
		sleep(1);
		printf("SUCCESS: File opened.\n");
		sleep(1);
	}else
	{
		printf("ERROR: Files can't be opened");
		return e_failure;
	}
	
	if(decode_magic_string_size(decInfo) == e_success)
	{
		printf("SUCCESS: Magic string length decoded.\n");
		sleep(1);
	}else
	{
		printf("ERROR: Magic String length decode error.");
		return e_failure;
	}
	if(decode_magic_string(decInfo) == e_success)
	{
		printf("SUCCESS: Decoding magic string is successful.\n");
		sleep(1);
		if(decode_secret_file_extn_size(decInfo) == e_success)
		{
			printf("SUCCESS: Decoding secret file extension size sucessful\n");
			sleep(1);
			if(decode_secret_file_extn(decInfo) == e_success)
			{
				printf("SUCCESS: Decoding secret file extension successful.\n");
				sleep(1);
				if(decode_secret_file_size(decInfo) == e_success)
				{
					printf("SUCCESS: Decoding secret file data size successful.\n");
					sleep(1);					
					if(decode_secret_file_data(decInfo) == e_success)
					{
						printf("SUCCESS: Decoding secret file data successful\n");
						sleep(1);
						return e_success;
					}else
					{							
						printf("ERROR: Encoding the secret file data is failure\n");
						return e_failure;
					}
				}else{
					printf("ERROR: Encoding the secret file size failure\n");
					return e_failure;
				}
			}else{
				printf("ERROR: Encoding the secret file extension failure\n");
				return e_failure;
			}
		}else{
			printf("ERROR: Encoding size of extensiom failure\n");
			return e_failure;
		}
	}else{
		printf("ERROR: Magic string encoding failure\n");
		return e_failure;
	}

}

Status decode_magic_string_size(DecodeInfo *decInfo)                       //function to decode magic string size
{
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    char buff[32];
    decInfo->data = 0x00000000;
    fread(buff,sizeof(char),32,decInfo->fptr_stego_image);
    decode_lsb_to_int(buff,&decInfo->data);
    //printf("Magic string size is %u\n",decInfo->data);
    return e_success;
}
Status decode_lsb_to_int(char *buff,int *data)                           //common function to decode size of all files
{
    int i;
    for(i=0;i<32;i++)
    {
	*data=*data|(buff[i]&1)<<i;
    }
}
Status decode_magic_string(DecodeInfo *decInfo)                            //function to decode magic string
{
    //printf("pointer pointing at %ld\n",ftell(decInfo->fptr_stego_image));
    char magic_data[decInfo->data];
    char buff[8];
    char temp;
    int i;
    for(i=0;i<decInfo->data;i++)
    {
	fread(buff,sizeof(char),8,decInfo->fptr_stego_image);
	if(decode_lsb_to_char(buff,&temp) == e_success)
	{
	    magic_data[i]=temp;
	}
	else return e_failure;
    }
    magic_data[i]='\0';
   // printf("%s\n",magic_data);
    return e_success;
}
Status decode_lsb_to_char(char *buff,char *data)                          //common function to decode data of all files
{
    int i;
    *data=0;
    for(i=7;i>=0;i--)
    {
	*data<<=1;
	if(buff[i]&1) 
	    *data|=1;
    }
    // printf("%c\n",*data);  
    if(i==-1) return e_success;
    else return e_failure;
}
Status decode_secret_file_extn_size(DecodeInfo *decInfo)                     //function to decode secret file extension size(4)
{
    //printf("pointer pointing at %ld\n",ftell(decInfo->fptr_stego_image));
    char buff[32];
    decInfo->secret_extn_capacity=0x00000000;
    fread(buff,sizeof(char),32,decInfo->fptr_stego_image);
    decode_lsb_to_int(buff,&decInfo->secret_extn_capacity);
    //printf("Secret extension size is %u\n",decInfo->secret_extn_capacity);
    return e_success;    
}
Status decode_secret_file_extn(DecodeInfo *decInfo)                       //function to decode secret file extension(.txt)    
{
    //printf("pointer pointing at %ld\n",ftell(decInfo->fptr_stego_image));
    char secret_data[decInfo->secret_extn_capacity];
    char buff[8];
    char temp;
    int i;
    for(i=0;i<decInfo->secret_extn_capacity;i++)
    {
	fread(buff,sizeof(char),8,decInfo->fptr_stego_image);
	if(decode_lsb_to_char(buff,&temp) == e_success)
	{
	    secret_data[i]=temp;
	}
	else return e_failure;
    }
    secret_data[i]='\0';
    //printf("%s\n",secret_data);
    return e_success;
}
Status decode_secret_file_size(DecodeInfo *decInfo)                      //function to decode over all secret file size
{
   // printf("pointer pointing at %ld\n",ftell(decInfo->fptr_stego_image));
    char buff[32];
    decInfo->secret_file_capacity=0x00000000;
    fread(buff,sizeof(char),32,decInfo->fptr_stego_image);
    decode_lsb_to_int(buff,&decInfo->secret_file_capacity);
    //printf("Secret file size is %u\n",decInfo->secret_file_capacity);
    return e_success;    

}
Status decode_secret_file_data(DecodeInfo *decInfo)                         //function to decode secret file data
{
    //printf("pointer pointing at %ld\n",ftell(decInfo->fptr_stego_image));
    char secret_file_data[decInfo->secret_file_capacity];
    char buff[8];
    char temp;
    int i;
    for(i=0;i<decInfo->secret_file_capacity;i++)
    {
	fread(buff,sizeof(char),8,decInfo->fptr_stego_image);
	if(decode_lsb_to_char(buff,&temp) == e_success)
	{
	    secret_file_data[i]=temp;
	}
	else return e_failure;
    }
    secret_file_data[i]='\0';
    //printf("%s\n",secret_file_data);
    fwrite(secret_file_data,sizeof(char),decInfo->secret_file_capacity,decInfo->fptr_output_image);
    return e_success;

}




