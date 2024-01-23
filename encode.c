#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <stdlib.h>
#include <unistd.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);                          // Seek to 18th byte

    fread(&width, sizeof(int), 1, fptr_image);                // Read the width (an int)
    //printf("width = %u\n", width);

    fread(&height, sizeof(int), 1, fptr_image);               // Read the height (an int)
    //printf("height = %u\n", height);
    return width * height * 3;                                // Return image capacity
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)                                  //opening all requored files
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");                 // Src Image file
    if (encInfo->fptr_src_image == NULL)                                            // Do Error handling
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
	return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");                       // Secret file
    if (encInfo->fptr_secret == NULL)                                               // Do Error handling
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
	return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");             // Stego Image file
    if (encInfo->fptr_stego_image == NULL)                                          // Do Error handling
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
	return e_failure;
    }
    return e_success;                                                              // No failure return e_success
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)           // Read and validate Encode args from argv 
{
    char buff[4];
    if(strcmp(strstr(argv[2],"."),".bmp")==0)
    {
	encInfo->src_image_fname = argv[2];
	encInfo->secret_fname=argv[3];
	strcpy(encInfo->extn_secret_file,strstr(encInfo->secret_fname,"."));
	//printf("File extension is %s\n",encInfo->extn_secret_file);
	if (argv[4]!=NULL)
	    encInfo->stego_image_fname=argv[4];
	else
	    encInfo->stego_image_fname="stego.bmp";
	return e_success;
    }
    else
	return e_failure;
}

uint get_magic_str_size()                                       //Get magic string size
{
    uint size=sizeof(MAGIC_STRING)-1;
    return size;
}
uint get_file_size(FILE *fptr)                                  // Get file size 
{
    uint size;
    fseek(fptr,0,SEEK_END);
    size=(uint)ftell(fptr);
    return size;
}

Status check_capacity(EncodeInfo *encInfo)                       //checking capacity of source file to encode data within same size
{
    uint magic_size      = get_magic_str_size();
    uint header_size     = 54;
    uint secret_file_ext = sizeof(encInfo->extn_secret_file);
    uint secret          = get_file_size(encInfo->fptr_secret);
    uint capacity    = encInfo->image_capacity-(magic_size+header_size+secret_file_ext+secret+MAX_MAGIC_STRING_LENGTH)*MAX_IMAGE_BUF_SIZE;

    if(encInfo->image_capacity >= capacity)
	return e_success;
    else
	return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_stego_image)                        //copy header file to encoded file
{
    char buff[53];
    fseek(fptr_src_image,0,SEEK_SET);
    fread(buff,sizeof(char),54,fptr_src_image);
    fwrite(buff,sizeof(char),54,fptr_stego_image);
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)                                                    //complete encoding
{
    if((copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == 0))
	{
		sleep(1);
		printf("SUCCESS: Copy header file is success\n");
		if ((encode_magic_string_size(get_magic_str_size(),encInfo) == 0))
		{
			sleep(1);
			printf("SUCCESS: Magic string length success\n");
			if((encode_magic_string(MAGIC_STRING,encInfo)==0))
			{
				sleep(1);
				printf("SUCCESS: Encoding magic string is successful\n");
				if((encode_secret_file_extn_size(MAX_FILE_SUFFIX,encInfo)==0))
				{
					sleep(1);
					printf("SUCCESS: Encoding secret file extension size sucessful\n");
					if((encode_secret_file_extn(encInfo->extn_secret_file,encInfo))==0)
					{
						sleep(1);
						printf("SUCCESS: Encoding secret file extension successful\n");
						if((encode_secret_file_data_size(get_file_size(encInfo->fptr_secret),encInfo))==0)
						{
							sleep(1);
							printf("SUCCESS: Encoding secret file data size successful\n");
    						if((encode_secret_file_data(encInfo)==0))
							{
								sleep(1);
								printf("SUCCESS: Encoding secret file data successful\n");
								if((copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image))==0)
								{
									sleep(1);
									printf("SUCCESS: Copy remaining data successful\n");
									return e_success;
								}else{
									printf("ERROR: Copy remaining data unsuccessful\n");
									return e_failure;
								}
							}else{
								printf("ERROR: Encoding secret file data size unsuccessful\n");
								return e_failure;
							}
						}else{
							printf("ERROR: Encoding secret file extension unsuccessful\n");
							return e_failure;
						}
					}else{
						printf("ERROR: Encoding secret file extension size unsucessful\n");
						return e_failure;
					}
				}else{
					printf("ERROR: Encoding secret file extension size unsucessful\n");
					return e_failure;
				}
			}else{
				printf("ERROR: Encoding magic string is unsuccessful\n");
				return e_failure;
			}
		}else{
			printf("ERROR: Encoding Magic string length unsuccessful\n");
			return e_failure;
		}
	}else{
		printf("ERROR: Copy header file is unsuccessful\n");
		return e_failure;
	}
}

Status encode_magic_string_size(uint size, EncodeInfo *encInfo)               //function to encode magic string size
{
    char buffer[32];
    fread(buffer,sizeof(char),32,encInfo->fptr_src_image);
    for(int i=0;i<32;i++)
    {
	buffer[i]=(buffer[i]&0xfe) | ((uint)(size & 1<<i) >> i);
	fwrite(&buffer[i],sizeof(char),1,encInfo->fptr_stego_image);
    }
    // printf("pointer pointing at %ld\n",ftell(encInfo->fptr_src_image));           //pointer pointing at
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)         //encoding magic string
{
    char image_buffer[MAX_IMAGE_BUF_SIZE];                         //buffer for replacing lsb bits ie.8 bytes for 1 one bit replacement
    int i,size=get_magic_str_size();
    for(i=0;i<size;i++)
    {
	//printf("%c\n",magic_string[i]);
	fread(image_buffer,sizeof(char),MAX_IMAGE_BUF_SIZE,encInfo->fptr_src_image);
	encode_byte_to_lsb(magic_string[i], image_buffer);
	fwrite(image_buffer,sizeof(char),MAX_IMAGE_BUF_SIZE,encInfo->fptr_stego_image);
    }
    // printf("pointer pointing at %ld\n",ftell(encInfo->fptr_src_image));
    if(i==size)
	return e_success;
    else
	return e_failure;
}
Status encode_byte_to_lsb(char data, char *image_buffer)                 //common function for all functions to encode data to LSB
{
    // printf("data is %c\n",data);
    int i;
    for(i=0;i<MAX_IMAGE_BUF_SIZE;i++)
    {
	image_buffer[i]=image_buffer[i]&(char)0xfe;
	if((data & 1<<i) !=0)
	    image_buffer[i]=image_buffer[i]|(char)0x01;
    }
    if(i==MAX_IMAGE_BUF_SIZE)
	return e_success;
    else 
	return e_failure;
}
Status encode_secret_file_extn_size(int file_extn_size,EncodeInfo *encInfo)//function to encode secret file exte size(i.e .txt size is 4)
{
    char buff[32];
    int i;
    fread(buff,sizeof(char),32,encInfo->fptr_src_image);
    for(i=0;i<32;i++)
    {
	buff[i]=buff[i]&0xfe;
	if((file_extn_size & 1<<i)!=0)
	    buff[i]=buff[i]|1;
	fwrite(&buff[i],sizeof(char),1,encInfo->fptr_stego_image);
    }
    // printf("pointer pointing at %ld\n",ftell(encInfo->fptr_src_image));
    if(i==32)
	return e_success;
    else
	return e_failure;
}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)      //function to encode secret file extension(i.e .txt)
{
    char buff[MAX_IMAGE_BUF_SIZE];
    int i;
    for(i=0;i<(int)strlen(encInfo->extn_secret_file);i++)
    {
	fread(buff,sizeof(char),MAX_IMAGE_BUF_SIZE,encInfo->fptr_src_image);
	encode_byte_to_lsb(file_extn[i],buff);
	fwrite(&buff,sizeof(char),MAX_IMAGE_BUF_SIZE,encInfo->fptr_stego_image);
    }
    // printf("pointer pointing at %ld\n",ftell(encInfo->fptr_src_image));
    if(i==(int)strlen(encInfo->extn_secret_file))
	return e_success;
    else
	return e_failure;
}
Status encode_secret_file_data_size(uint size, EncodeInfo *encInfo)                 //function to encode overall secret file size
{
    char buff[32];
    fread(buff,sizeof(char),32,encInfo->fptr_src_image);
    for(int i=0;i<32;i++)
    {
	buff[i]=(buff[i]&0xfe) | ((uint)(size & 1<<i) >> i);
	fwrite(&buff[i],sizeof(char),1,encInfo->fptr_stego_image);
    }
    //  printf("pointer pointing at %ld\n",ftell(encInfo->fptr_src_image));
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)                     //function to encode secret file data
{
    char data_buffer[8];
    int i, buff=get_file_size(encInfo->fptr_secret);
    //printf("data buffer is %d\n",buff);
    char secret_buffer[buff];
    rewind(encInfo->fptr_secret);
    fread(secret_buffer,sizeof(char),buff,encInfo->fptr_secret);
    /*  for(i=0;i<buff;i++)
	{
	printf("%c",secret_buffer[i]);
	}
	printf("\n");
     */
    for(i=0;i<buff;i++)
    {
	//	printf("size of file is %d\n",buff);
	fread(data_buffer,sizeof(char),8,encInfo->fptr_src_image);
	encode_byte_to_lsb(secret_buffer[i], data_buffer);
	fwrite(data_buffer,sizeof(char),8,encInfo->fptr_stego_image);
    }
    // printf("pointer pointing at %ld\n",ftell(encInfo->fptr_src_image));
    if(i==buff)
	return e_success;
    else
	return e_failure;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)           //function to copy reamining bytes from the source file
{
    char buff;
    while(fread(&buff,sizeof(char),1,fptr_src)>0)
	fwrite(&buff,sizeof(char),1,fptr_dest);
    return e_success;
}
