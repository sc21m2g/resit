#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define IDENTICAL 0;
#define DIFFERENT 0;
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
typedef struct {
    FILE* inp_file;
    FILE* outp_file;
    unsigned char* input_pgm_data;
} pgm_info_t;

static pgm_info_t pgm_info1;
static pgm_info_t pgm_info2;

typedef struct {
    unsigned int w;         // 图像宽度
    unsigned int h;         // 图像高度
    unsigned int max_gray;  // 最大灰度值
    unsigned int size;      // 像素数据大小
} pgm_property_t;

int pgm_parse_image_property(FILE* f, pgm_property_t* prop) {
    assert(f);
    assert(prop);

    int scanCount = fscanf(f, " %u %u %u", &prop->w, &prop->h, &prop->max_gray);
    if ((scanCount != 3) ||
        (prop->w < MIN_IMAGE_DIMENSION) ||
        (prop->w > MAX_IMAGE_DIMENSION) ||
        (prop->h < MIN_IMAGE_DIMENSION) ||
        (prop->h > MAX_IMAGE_DIMENSION) ||
        (prop->max_gray != 255)) {
        return EXIT_Bad_Dimensions;
    }

    return 0;
}

int pgm_read_image_data(FILE* f, pgm_property_t* prop, unsigned char** img_buff) {
    assert(f);
    assert(prop);

    prop->size = prop->w * prop->h * sizeof(unsigned char);
    *img_buff = (unsigned char*)malloc(prop->size);
    if (*img_buff == NULL) {
        return EXIT_Image_Malloc_Failed;
    }

    for (unsigned char* next_gray_val = *img_buff; next_gray_val < *img_buff + prop->size; next_gray_val++) {
        int gray_val = -1;
        int cnt = fscanf(f, " %u", &gray_val);

        if ((cnt != 1) || (gray_val < 0) || (gray_val > 255)) {
            return EXIT_BAD_FILE_NAME;
        }

        *next_gray_val = (unsigned char)gray_val;
    }

    return 0;
}

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

    if(*magic_Number == MAGIC_NUMBER_ASCII_PGM || (*magic_Number == MAGIC_NUMBER_RAW_PGM))
        return 0;

	return EXIT_BAD_MAGIC_NUMBER;
}

int comparePGM(const char* file1, const char* file2) {
    /*FILE* fp1 = fopen(file1, "rb");
    if (fp1 == NULL) {
        printf("无法打开文件：%s\n", file1);
        return 0;
    }

    FILE* fp2 = fopen(file2, "rb");
    if (fp2 == NULL) {
        printf("无法打开文件：%s\n", file2);
        fclose(fp1);
        return 0;
    }*/
    memset(&pgm_info1, 0, sizeof(pgm_info_t));
    memset(&pgm_info2, 0, sizeof(pgm_info_t));

    pgm_info1.inp_file = fopen(file1, "rb");
    if (pgm_info1.inp_file == NULL)
    {
        printf("ERROR: Bad File Name (%s)\n",pgm_info1.inp_file);
        return EXIT_BAD_FILE_NAME;
    }
    pgm_info2.inp_file = fopen(file2, "rb");
    if (pgm_info2.inp_file == NULL)
    {
        printf("ERROR: Bad File Name (%s)\n", pgm_info2.inp_file);
        return EXIT_BAD_FILE_NAME;
    }

    if(pgm_check_magic_number(pgm_info1.inp_file) > 0)
    {
        printf("ERROR: Bad Magic Number (%s)\n",file1);
        fclose(pgm_info1.inp_file);
        fclose(pgm_info2.inp_file);
        return EXIT_BAD_MAGIC_NUMBER;
    }

    if(pgm_check_magic_number(pgm_info2.inp_file) > 0)
    {
        printf("ERROR: Bad Magic Number (%s)\n", file2);
        fclose(pgm_info1.inp_file);
        fclose(pgm_info2.inp_file);
        return EXIT_BAD_MAGIC_NUMBER;
    }
    
    pgm_property_t prop1, prop2;
    int result1 = pgm_parse_image_property(pgm_info1.inp_file, &prop1);
    int result2 = pgm_parse_image_property(pgm_info2.inp_file, &prop2);

    if (result1 != 0) {
        fclose(pgm_info1.inp_file);
        fclose(pgm_info2.inp_file);
        printf("ERROR: Bad Dimensions (%s)\n", file1);
        return EXIT_Bad_Dimensions;
    }

    if (result2 != 0) {
        fclose(pgm_info1.inp_file);
        fclose(pgm_info2.inp_file);
        printf("ERROR: Bad Dimensions (%s)\n", file2);
        return EXIT_Bad_Dimensions;
    }

    if (prop1.w != prop2.w || prop1.h != prop2.h) {
        printf("DIFFERENT\n");
        fclose(pgm_info1.inp_file);
        fclose(pgm_info2.inp_file);
        return DIFFERENT;
    }

    unsigned char* img_buff1 = NULL;
    unsigned char* img_buff2 = NULL;
    int result3 = pgm_read_image_data(pgm_info1.inp_file, &prop1, &img_buff1);
    int result4 = pgm_read_image_data(pgm_info2.inp_file, &prop2, &img_buff2);

    if (result3 != 0) {
        printf("ERROR: Bad Data (%s)\n", file1);
        if (img_buff1 != NULL) {
            free(img_buff1);
        }
        fclose(pgm_info1.inp_file);
        fclose(pgm_info2.inp_file);
        return EXIT_BAD_DATA;
    }

    if (result4 != 0) {
        printf("ERROR: Bad Data (%s)\n", file2);
        if (img_buff1 != NULL) {
            free(img_buff1);
        }
        if (img_buff2 != NULL) {
            free(img_buff2);
        }
        fclose(pgm_info1.inp_file);
        fclose(pgm_info2.inp_file);
        return EXIT_BAD_DATA;
    }

    for (int i = 0; i < prop1.h; i++) {
        for (int j = 0; j < prop1.w; j++) {
            int index = i * prop1.w + j;
            if (img_buff1[index] != img_buff2[index]) {
                printf("DIFFERENT\n");
                break;
            }
            
        }
    }
    printf("IDENTICAL\n");
    // img_buff1 和 img_buff2 存储着读取到的像素数据

    // 释放内存和关闭文件
    free(img_buff1);
    free(img_buff2);
    fclose(file1);
    fclose(file2);

    return 0;
}

int main(int argc, char* argv[]) {
    /* check for correct number of arguments */
    if (argc == 1)
    { /* wrong arg count */
        /* print an error message        */
        printf("Usage: ./pgmComp inputImage.pgm inputImage.pgm\n");
        /* and return an error code      */
        return 0;
    } /* wrong arg count */

    if (argc != 3)
    {
        printf("ERROR: Bad Argument Count\n");
        return EXIT_WRONG_ARG_COUNT;
        
    }
    const char* file1 = argv[1];
    const char* file2 = argv[2];
    

    return comparePGM(file1,file2);
}