/* 
Sample Input  : For encoding : source file - beautiful.bmp
                               secret data file - secret.txt
							   destination file - stego.bmp
				For decoding : source file - stego.bmp
							   destination file - outtput.txt
Sample Output : Encoded file : stego.bmp
                Output data fetched after decoding : output.txt
*/


#include<stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <stdlib.h>
#include <unistd.h>

OperationType check_operation(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
	return e_encode;                                                    //0 is return
    else if(strcmp(argv[1],"-d")==0)
	return e_decode;                                                	//1 is returned
    else 
	return e_unsupported;
}

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    if(argc>=3)
    {
	if(check_operation(argv)==e_encode)                               	    //encoding part
	{	    
	    if(read_and_validate_encode_args(argv,&encInfo) == e_success)
	    {
		sleep(1);
		printf("SUCCESS: Read and validate function\n");
		if(open_files(&encInfo) == e_success)
		{
			sleep(1);
		    printf("SUCCESS: open file function\n");
		    encInfo.image_capacity=get_image_size_for_bmp(encInfo.fptr_src_image);
		    if(check_capacity(&encInfo) == e_success)
		    {
				sleep(1);
				printf("SUCCESS: Check capacity\n");
				if((do_encoding(&encInfo) == e_success)){
					system("eog output.bmp");
					sleep(1);
					printf("SUCCESS: Encoding\n");
				}
		    }
		}
	    }
	}
	else if(check_operation(argv) == e_decode)                       	    //decoding part 
	{
	    if(read_and_validate_decode_args(argv,&decInfo) == e_success)
	    {
		sleep(1);
		printf("SUCCESS: Read and validate function\n");
		if(open_decoding_files(&decInfo) == e_success)
		{
			sleep(1);
		    printf("SUCCESS: open file function\n");
		    if((do_decoding(&decInfo) == e_success)){
				sleep(1);
				printf("SUCCESS: Decoding\n");
			}
		}
	    }
	}
    }

    else                                                                            
    {
	printf("Please Enter a Valid Input\n");
	printf("Encoding:./a.out -e beautiful.bmp secret.txt\n");
    }
    return 0;
}
