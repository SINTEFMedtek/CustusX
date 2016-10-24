#ifndef TEXTURE_H
#define TEXTURE_H

template <typename TYPE>
TYPE *generateTexture(unsigned int x, unsigned int y, unsigned int z, TYPE color)
{
	int numberOfColorComponents = 4;
	unsigned int data_size = x*y*z;
	TYPE *data = new TYPE[data_size];
	memset(data, color, data_size);

	//print<unsigned char, float>(data, data_size);
};


unsigned char *checkerboard(unsigned int width, unsigned int height, unsigned int depth);

#endif // TEXTURE_H
