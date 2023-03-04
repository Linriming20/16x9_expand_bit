#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 与app约定的区域数据宽x高，修改后需要重新写对应的函数  
#define SRC_REGION_W 	   16
#define SRC_REGION_H 	    9

#if 0
#define MD_W_MACRO_CNT     16
#define MD_H_MACRO_CNT     9
#elif  0
#define MD_W_MACRO_CNT     48
#define MD_H_MACRO_CNT     27
#elif  0
#define MD_W_MACRO_CNT     80
#define MD_H_MACRO_CNT     45
#elif  1
#define MD_W_MACRO_CNT     90
#define MD_H_MACRO_CNT     51
#endif

char g_mdMacroData[MD_H_MACRO_CNT][(MD_W_MACRO_CNT+7)/8] = {0};   // 用于匹配移动侦测比较  


static int i_16x9_expand_region(const unsigned char *src, char **dst, unsigned int dst_w, unsigned int dst_h);

int main(int argc, char *argv[])
{
	unsigned char src[18] = {0x80, 0x00, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

	i_16x9_expand_region(src, (char **)g_mdMacroData, MD_W_MACRO_CNT, MD_H_MACRO_CNT);

    return 0;
}


static int i_16x9_expand_region(const unsigned char *src, char **dst, unsigned int dst_w, unsigned int dst_h)
{
    unsigned int iRegionMacroCnt = 0;
    float multi_h = (float)dst_w/SRC_REGION_W;
    float multi_v = (float)dst_h/SRC_REGION_H;
	int dst_line_bytes = (dst_w+7)/8;

	printf("multi_h: %f \t multi_v: %f\n", multi_h, multi_v);
    for(int i = 0; i < dst_h; i++)
    {
        for(int j = 0; j < dst_w; j++)
        {
			int src_line = (int)(i / multi_v);
			int src_row = (int)(j / multi_h / 8);
			int src_byte_index = src_line * 2 + src_row;
			int src_bit_index = 7 - (int)((j % (int)(multi_h * 8))/multi_v);
			int val = (src[src_byte_index] >> src_bit_index) & 0x01;

			printf("i: %d \t j: %d \t src_line: %d \t  src_row: %d \t src_byte_index: %d \t src_bit_index: %d \t val: %d\n", i, j, src_line, src_row, src_byte_index, src_bit_index, val);


			//printf("dst_line_bytes: %d\n", dst_line_bytes);
            if (val)
            {
                *((char *)dst +i*dst_line_bytes+j/8) |= (1 << (j%8));
                iRegionMacroCnt++;
            }
            else
            {
				if((*((char *)dst +i*dst_line_bytes+j/8) & (1 << (j%8))))
					printf("<><>\n");
				
					*((char *)dst +i*dst_line_bytes+j/8) &= ~(1 << (j%8));
            }
        }
    }

#if 1 //ndef RELEASE_VER
    printf("\n");
    for(int i = 0; i < dst_h; i++)
    {
        for(int j = 0; j < dst_w; j++)
        {
            printf("%c", (*((char *)dst +i*dst_line_bytes+j/8) >> (j%8)) & 0x01 ? 'o' : '-');
        }
        printf("\n");
    }
    printf("\n");
#endif
    return iRegionMacroCnt;
}

