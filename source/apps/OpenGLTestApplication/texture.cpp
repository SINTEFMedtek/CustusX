#include "texture.h"

#include <stdio.h>
#include <string.h>
#include <iostream>

#define MAX_WIDTH 100
#define MAX_HEIGTH 100
#define MAX_DEPTH 100
#define NUM_COMPONENTS 4
static unsigned char texture_data[MAX_WIDTH][MAX_HEIGTH][MAX_DEPTH][NUM_COMPONENTS];

template <typename IN, typename OUT>
void print(IN *data, unsigned int data_size)
{
	for(int i=0; i<data_size; ++i)
	{
		std::cout << " " << (OUT)data[i];
	}
	std::cout << std::endl;
}

unsigned char *checkerboard(unsigned int width, unsigned int height, unsigned int depth)
{
	//TODO her må det være noe feil...
	int i,j,k,c;

	for(i=0; i<height; ++i)
	{
		for(j=0; j<width; ++j)
		{
			for(k=0; k<depth; ++k)
			{
				c=((((i&0x8)==0)^((j&0x8))==0))*255;
				texture_data[i][j][k][0] = (unsigned char) c;
				texture_data[i][j][k][1] = (unsigned char) c;
				texture_data[i][j][k][2] = (unsigned char) c;
				texture_data[i][j][k][3] = (unsigned char) 255;
			}
		}
	}

	return &texture_data[0][0][0][0];
}
