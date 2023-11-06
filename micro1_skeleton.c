#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"


#include <math.h>
#include <stdio.h>
#include <float.h>


void mirror_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void grayScale_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void sobelFiltering_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);

int main()
{
 
	int height;
	int width;
	int channel;

	char command;

 	printf("Take a picture? (y/n)\n");
 	scanf("%c", &command);

 	if(command == 'n')
 		exit(1);


 	printf("Cheeze !\r\n");
	system("libcamera-still --width 640 --height 480 -o image.bmp");
 	
 	
 	unsigned char* imgIn = stbi_load("image.bmp", &width, &height, &channel, 3);


	unsigned char* imgOut_mirror = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
	unsigned char* imgOut_grayScale = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
	unsigned char* imgOut_sobelFiltering = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
 	
	mirror_transform(imgIn, height, width, channel, imgOut_mirror);
	grayScale_transform(imgIn, height, width, channel, imgOut_grayScale);
	sobelFiltering_transform(imgOut_grayScale, height, width, channel, imgOut_sobelFiltering);


	stbi_write_bmp("image_mirror.bmp", width, height, channel, imgOut_mirror);
	stbi_write_bmp("image_grayScale.bmp", width, height, channel, imgOut_grayScale);
	stbi_write_bmp("image_sobelFiltering.bmp", width, height, channel, imgOut_sobelFiltering);
	
	stbi_image_free(imgIn);
	free(imgOut_mirror);
	free(imgOut_grayScale);
	free(imgOut_sobelFiltering);
 

	return 0;
}

void mirror_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
    int i, j, c;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            for (c = 0; c < channel; c++) {
                int original_index = (i * width + j) * channel + c;
                int mirror_index = (i * width + (width - 1 - j)) * channel + c;
                out[mirror_index] = in[original_index];
            }
        }
    }
}

void grayScale_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            int r = in[(i * width + j) * channel + 0];
            int g = in[(i * width + j) * channel + 1];
            int b = in[(i * width + j) * channel + 2];
            int gray = (r + g + b) / 3;
            out[(i * width + j) * channel + 0] = gray;
            out[(i * width + j) * channel + 1] = gray;
            out[(i * width + j) * channel + 2] = gray;
        }
    }
}

void sobelFiltering_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
    int i, j, c;
    int x_filter[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
    int y_filter[3][3] = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            for (c = 0; c < channel; c++) {
                int Gx = 0, Gy = 0;
                for (int x = -1; x <= 1; x++) {
                    for (int y = -1; y <= 1; y++) {
                        //zero-padding and convolution
                        int row = i + x;
                        int col = j + y;

                        if (row >= 0 && row < height && col >= 0 && col < width) {
                            int index = (row * width + col) * channel + c;
                            Gx += x_filter[x + 1][y + 1] * in[index];
                            Gy += y_filter[x + 1][y + 1] * in[index];
                        }
                    }
                }
                int index = (i * width + j) * channel + c;
                int gradient = sqrt(Gx * Gx + Gy * Gy);
                out[index] = gradient;
            }
        }
    }
}
