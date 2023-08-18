#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* library for I/O routines        */
#include <stdio.h>
/* library for memory routines     */
#include <stdlib.h>
#include <assert.h>
#include <string.h>

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
//TODO::
/*
1   ERROR: Bad Argument Count	Program given wrong # of arguments
2	ERROR: Bad File Name (fname)	Program failed to open a file stream
3	ERROR: Bad Magic Number (fname)	Program failed on a magic number
4	ERROR: Bad Comment Line (fname)	Program failed on comment line
5	ERROR: Bad Dimensions (fname)	Program failed on image dimensions
6	ERROR: Bad Max Gray Value (fname)	Program failed on max gray value
7	ERROR: Image Malloc Failed	Malloc failed for image allocation
8	ERROR: Bad Data (fname)	Reading in data failed
9	ERROR: Output Failed (fname)	Writing out data failed
100	ERROR: Miscellaneous (text description)	Any other error that is detected.
*/

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


int pgm_check_magic_number(FILE* f);

int pgm_check_comment_line(FILE* f);

int pgm_read_image_data(FILE* f, pgm_property_t* prop, unsigned char** img_buff);

int pgm_convert_2_matrix(FILE* src_file, FILE* dest_file, int width, int height, int max_gray);

int pgm_parse_image_property(FILE* f, pgm_property_t* prop);

int do_exception(int err, char* inputfile, char* outputfile);

void resource_release();
/***********************************/
/* main routine                    */
/*                                 */
/* CLI parameters:                 */
/* argv[0]: executable name        */
/* argv[1]: input file name        */
/* argv[2]: output file name       */
/* returns 0 on success            */
/* non-zero error code on fail     */
/***********************************/
static pgm_info_t pgm_info;
int main(int argc, char** argv)
{
	/* check for correct number of arguments */
	if (argc == 1)
	{ /* wrong arg count */
		/* print an error message        */
		printf("Usage: ./pgma2b inputImage.pgm outputImage.pgm\n");
		/* and return an error code      */
		return 0;
	} /* wrong arg count */

	int err = 0;
	if (argc != 3)
	{
		printf("ERROR: Bad Argument Count\n");
		err = EXIT_WRONG_ARG_COUNT;
		
	}

	memset(&pgm_info, 0, sizeof(pgm_info_t));

	pgm_info.inp_file = fopen(argv[1], "r");
	/* if it fails, return error code        */
	if (pgm_info.inp_file == NULL)
	{
		err = EXIT_BAD_FILE_NAME;
		goto do_err;
	}

	//magic number
	err = pgm_check_magic_number(pgm_info.inp_file);
	if (err != EXIT_NO_ERRORS)
		goto do_err;

	// 
	err = pgm_check_comment_line(pgm_info.inp_file);
	if (err != EXIT_NO_ERRORS)
		goto do_err;
	pgm_property_t  prop = { 0 };
	// 
	err = pgm_parse_image_property(pgm_info.inp_file, &prop);
	if (err != EXIT_NO_ERRORS)
		goto do_err;

	err = pgm_read_image_data(pgm_info.inp_file, &prop, &pgm_info.input_pgm_data);
	if (err != EXIT_NO_ERRORS)
		goto do_err;

	pgm_info.outp_file = fopen(argv[2], "w");
	if (pgm_info.outp_file == NULL)
	{
		//TODO::ERROR: Bad File Name (fname)
		err = EXIT_BAD_FILE_NAME;
		goto do_err;
	}

	err = pgm_convert_2_matrix(pgm_info.outp_file, pgm_info.outp_file, prop.w, prop.h, prop.max_gray);
	if (err != EXIT_NO_ERRORS)
		goto do_err;
do_err:
	do_exception(err, argv[1], argv[2]);

	resource_release();
	exit(err);
	return 0;
}

void resource_release()
{
	if (pgm_info.inp_file)
		fclose(pgm_info.inp_file);

	if (pgm_info.outp_file)
		fclose(pgm_info.outp_file);

	if (pgm_info.input_pgm_data)
		free(pgm_info.input_pgm_data);
}

// ERROR
int pgm_check_magic_number(FILE* f)
{
	/* the magic number		         */
	/* stored as two bytes to avoid	         */
	/* problems with endianness	         */
	/* Raw:    0x5035 or P5		         */
	/* ASCII:  0x5032 or P2		         */
	unsigned char magic_number[2] = { '0','0' };
	unsigned short* magic_Number = (unsigned short*)magic_number;
	/* read in the magic number              */
	magic_number[0] = getc(f);
	magic_number[1] = getc(f);

	/* sanity check on the magic number      */
	if (*magic_Number != MAGIC_NUMBER_ASCII_PGM)
	{ /* failed magic number check   */
		/* be tidy: close the file       */
		/* and return                    */
		return EXIT_BAD_MAGIC_NUMBER;
	} /* failed magic number check   */

	return 0;
}

int pgm_check_comment_line(FILE* f)
{
	assert(f);
	fscanf(f, " ");
	/* check for a comment line              */
	char nextChar = fgetc(f);
	if (nextChar == '#')
	{ /* comment line                */
		/* allocate buffer               */
		char commentLine[MAX_COMMENT_LINE_LENGTH] = { 0 };
		/* fgets() reads a line          */
		/* capture return value          */
		char* commentString = fgets(commentLine, MAX_COMMENT_LINE_LENGTH, f);
		/* NULL means failure            */
		if (commentString == NULL)
		{ /* NULL comment read   */
			//TODO:: ERROR: Bad Comment Line (fname)
			//TODO::
			//return EXIT_Bad_Comment_Line;
		} /* NULL comment read   */
	} /* comment line */
	else
	{ /* not a comment line */
		/* put character back            */
		ungetc(nextChar, f);
		//return EXIT_Bad_Comment_Line;
	} /* not a comment line */

	return 0;
}

int pgm_parse_image_property(FILE* f, pgm_property_t* prop)
{
	assert(f);
	assert(prop);

	int scanCount = fscanf(f, " %u %u %u", &prop->w, &prop->h, &prop->max_gray);
	if (
		(scanCount != 3) ||
		(prop->w < MIN_IMAGE_DIMENSION) ||
		(prop->w > MAX_IMAGE_DIMENSION) ||
		(prop->h < MIN_IMAGE_DIMENSION) ||
		(prop->h > MAX_IMAGE_DIMENSION) ||
		(prop->max_gray != 255)
		)
	{   //TODO:: ERROR: Bad Dimensions (fname)
		//RROR: Bad Max Gray Value (fname)
		return  EXIT_Bad_Dimensions;
		//return  EXIT_Bad_Max_Gray_Value;
	}

	return 0;
}

int pgm_read_image_data(FILE* f, pgm_property_t* prop, unsigned char** img_buff)
{
	assert(f);
	assert(prop);

	/* allocate the data pointer             */
	prop->size = prop->w * prop->w * sizeof(unsigned char);
	*img_buff = (unsigned char*)malloc(prop->size);
	/* sanity check for memory allocation    */
	if (img_buff == NULL)
	{
		//TODO:: ERROR: Image Malloc Failed
		return EXIT_Image_Malloc_Failed;
	}
	/* pointer for efficient read code       */
	for (unsigned char* next_gray_val = *img_buff; next_gray_val < *img_buff + prop->size; next_gray_val++)
	{ /* per gray value */
		/* read next value               */
		int gray_val = -1;
		int cnt = fscanf(f, " %u", &gray_val);

		/* sanity check	                 */
		if ((cnt != 1) || (gray_val < 0) || (gray_val > 255))
		{ /* fscanf failed */

			return EXIT_BAD_FILE_NAME;
		} /* fscanf failed */

		/* set the pixel value           */
		*next_gray_val = (unsigned char)gray_val;
	} /* per gray value */

	return 0;
}


int pgm_convert_2_matrix(FILE* src_file, FILE* dest_file, int width, int height, int max_gray) {
	// 写入二进制文件头部分
	fprintf(dest_file, "P5\n%d %d\n%d\n", width, height, max_gray);

	// 分配内存保存所有像素值
	int num_pixels = width * height;
	unsigned char* pixels = (unsigned char*)malloc(num_pixels * sizeof(unsigned char));
	if (pixels == NULL) {
		// TODO: 错误处理，内存分配失败
		return EXIT_Image_Malloc_Failed;
	}

	// 逐个像素读取和写入
	for (int i = 0; i < num_pixels; i++) {
		if (fscanf(src_file, "%hhu", &pixels[i]) != 1) {
			// TODO: 错误处理，读取像素值失败
			free(pixels);
			return EXIT_BAD_DATA;
		}
	}

	// 将像素值以二进制形式写入目标文件
	if (fwrite(pixels, sizeof(unsigned char), num_pixels, dest_file) < num_pixels) {
		// TODO: 错误处理，写入像素值失败
		free(pixels);
		return EXIT_Output_Failed;
	}

	// 释放内存并返回成功状态
	free(pixels);
	return 0;
}

// //TODO:
int do_exception(int err, char* inputfile, char* outputfile)
{
	//fclose(f);
	switch (err)
	{
	case EXIT_WRONG_ARG_COUNT:
		printf("ERROR: Bad Arguments\n");
		break;
	case EXIT_BAD_FILE_NAME:
		printf("ERROR: Bad File Name (%s)\n", inputfile);
		break;
	case EXIT_BAD_MAGIC_NUMBER:
		printf("ERROR: Bad Magic Number (%s)\n", inputfile);
		break;
	case EXIT_Bad_Comment_Line:
		printf("ERROR: Bad Comment Line (%s)\n", inputfile);
		break;
	case EXIT_Bad_Dimensions:
		printf("ERROR: Bad Dimensions (%s)\n", inputfile);
		break;
	case EXIT_Bad_Max_Gray_Value:
		printf("ERROR: Bad Max Gray Value (%s)\n", inputfile);
		break;
	case EXIT_Image_Malloc_Failed:
		printf("ERROR: Image Malloc Failed\n");
		break;
	case EXIT_Output_Failed:
		printf("ERROR: Output Failed (%s)\n", outputfile);
		break;
	case EXIT_ANY_OTHRER_ERR:
		printf("ERROR: Miscellaneous \n");
		break;
	default:
		printf("ECHOED\n");
		break;
	}
	return 0;
}