#include "texture.h"

#include <stdio.h>
#include <string.h>
#include <iostream>

template <typename IN, typename OUT>
void print(IN *data, unsigned int data_size)
{
	for(int i=0; i<data_size; ++i)
	{
		std::cout << " " << (OUT)data[i];
	}
	std::cout << std::endl;
}

unsigned char *generateTexture(unsigned int x, unsigned int y, unsigned int z, float color)
{
	unsigned int data_size = x*y*z;
	unsigned char *data = new unsigned char[data_size];
	memset(data, color, data_size);

	//print<unsigned char, float>(data, data_size);
}
