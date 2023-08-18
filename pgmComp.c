#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IDENTICAL 0
#define DIFFERENT 0
#define EXIT_NO_ERRORS 0
#define EXIT_WRONG_ARG_COUNT 1
#define EXIT_BAD_FILE_NAME 2
#define EXIT_BAD_MAGIC_NUMBER 3
#define EXIT_Bad_Comment_Line 4
#define EXIT_Bad_Dimensions 5
#define EXIT_Bad_Max_Gray_Value 6
#define EXIT_Image_Malloc_Failed 7
#define EXIT_BAD_DATA 8
#define EXIT_Output_Failed 9
#define EXIT_ANY_OTHRER_ERR 100 

#define MAGIC_NUMBER_RAW_PGM 0x3550
#define MAGIC_NUMBER_ASCII_PGM 0x3250
#define MIN_IMAGE_DIMENSION 1
#define MAX_IMAGE_DIMENSION 65536
#define MAX_COMMENT_LINE_LENGTH 128

typedef struct {
	int w;
	int h;
	int max_gray;
	int size;
}pgm_property_t;

typedef struct {
	FILE* inp_file;
	FILE* outp_file;
	unsigned char* input_pgm_data;
} pgm_info_t;

static pgm_info_t pgm_info1;
static pgm_info_t pgm_info2;
int main(int argc, char** argv)
{
	/* check for correct number of arguments */
	if (argc == 1)
	{ /* wrong arg count */
		/* print an error message        */
		printf("Usage: ./pgmComp inputImage.pgm inputImage.pgm\n");
		/* and return an error code      */
		return 0;
	} /* wrong arg count */

	int err = 0;
	if (argc != 3)
	{
		/*err = EXIT_WRONG_ARG_COUNT;
		goto do_err;*/
		return EXIT_WRONG_ARG_COUNT;
	}

	memset(&pgm_info1, 0, sizeof(pgm_info_t));
	memset(&pgm_info2, 0, sizeof(pgm_info_t));

	pgm_info1.inp_file = fopen(argv[1], "rb");
	pgm_info2.inp_file = fopen(argv[1], "rb");
	/* if it fails, return error code        */
	if (pgm_info1.inp_file == NULL || pgm_info2.inp_file == NULL)
	{
		/*err = EXIT_BAD_FILE_NAME;
		goto do_err;*/
		return EXIT_BAD_FILE_NAME;
	}
	int result = comparePGMFiles(pgm_info1.inp_file, pgm_info2.inp_file);
	if (result) {
		printf("The two files were identical\n");
		return IDENTICAL;
	}
	else {
		printf("The two files were not identical\n");
		return DIFFERENT;
	}

	return 0;

}

int comparePGMFiles(char* file1, char* file2) {

    // 判断文件大小是否相同
	fseek(file1, 0, SEEK_END);
	fseek(file2, 0, SEEK_END);
	long size1 = ftell(file1);
	long size2 = ftell(file2);
	fseek(file1, 0, SEEK_SET);
	fseek(file2, 0, SEEK_SET);
	if (size1 != size2) {
		fclose(file1);
		fclose(file2);
		return 0;
	}

    // 判断文件内容是否相同
    char* buffer1 = (char*)malloc(size1 * sizeof(char));
    char* buffer2 = (char*)malloc(size2 * sizeof(char));
    fread(buffer1, sizeof(char), size1, file1);
    fread(buffer2, sizeof(char), size2, file2);
	int result;
	result = memcmp(buffer1, buffer2, size1) == 0;

    // 释放内存，关闭文件
    free(buffer1);
    free(buffer2);
    fclose(file1);
    fclose(file2);

    return result;
}
//
//int main() {
//    char file1[100];
//    char file2[100];
//
//    printf("请输入第一个PGM文件的路径：");
//    scanf_s("%s", file1);
//
//    printf("请输入第二个PGM文件的路径：");
//    scanf_s("%s", file2);
//
//    int result = comparePGMFiles(file1, file2);
//
    
//}