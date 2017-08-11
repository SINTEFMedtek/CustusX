#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <string.h>
#include <iostream>

//OpenGL
#include <GL/glew.h>
#include <glut.h> //Framework on Mac

//------------------------------------------------------------------------------------

#define MAX_TEXTURE_SIZE 1000
static unsigned char generated_texture[MAX_TEXTURE_SIZE];
//static float generated_texture[MAX_TEXTURE_SIZE];

template <typename TYPE>
TYPE *generateTexture(unsigned int x, unsigned int y, unsigned int z,
					  TYPE red, TYPE green, TYPE blue, TYPE alpha,
					  bool debug=false)
{
	int numberOfColorComponents = 4;
	unsigned int data_size = x*y*z*numberOfColorComponents;

	if(data_size > MAX_TEXTURE_SIZE)
		std::cout << "-----------------------> Error: trying to create texture bigger than the max size" << std::endl;

	for(int i=0; i<data_size; )
	{
		generated_texture[i++] = (TYPE)red;
		generated_texture[i++] = (TYPE)green;
		generated_texture[i++] = (TYPE)blue;
		generated_texture[i++] = (TYPE)alpha;

	}

	if(debug)
	{
		std::cout.precision(3);
		for(int i=0; i<data_size; ++i)
		{
			std::cout << ' ' << (TYPE) generated_texture[i];
		}
		std::cout << std::endl;
	}

	return generated_texture;
};


//------------------------------------------------------------------------------------

#endif // TEXTURE_H
