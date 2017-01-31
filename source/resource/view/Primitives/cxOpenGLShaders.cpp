#include "cxOpenGLShaders.h"

#include <QString>

namespace cx
{

const std::string getVSReplacement_dec(std::string vtk_dec, int numberOfUploadedTextures)
{
	QString temp;

	if(numberOfUploadedTextures != 0)
	{
		temp = QString(
			 "%1"
			 "\n\n"
			 "//CX: adding input and output variables for texture coordinates\n"
			 "const int number_of_textures = %4;\n"
			 "in vec3 %2[number_of_textures];\n"
			 "out vec3 %3[number_of_textures];\n"
			   )
			   .arg(vtk_dec.c_str())
			   .arg(VS_In_Vec3_TextureCoordinate.c_str())
			   .arg(VS_Out_Vec3_TextureCoordinate.c_str())
			   .arg(numberOfUploadedTextures);
	}
	else
	{
		//If there are no textures present we do not need a vertexshader
		temp = vtk_dec.c_str();
	}

	const std::string retval = temp.toStdString();
	return retval;
}


const std::string getVSReplacement_impl(std::string vtk_impl, int numberOfUploadedTextures)
{
	QString temp;

	if(numberOfUploadedTextures != 0)
	{
		temp = QString(
			 "%1"
			 "\n"
			 "%2 = %3;\n"
			   )
			   .arg(vtk_impl.c_str())
			   .arg(VS_Out_Vec3_TextureCoordinate.c_str())
			   .arg(VS_In_Vec3_TextureCoordinate.c_str());
	}
	else
	{
		//If there are no textures present we do not need a vertexshader
		temp = vtk_impl.c_str();
	}

	const std::string retval = temp.toStdString();
	return retval;
}

/**
 * @brief getSampleLutImplementation generates code which will sample the LUT
 * WITHOUT using a for loop. The OpenGL specification states that for loops are allowed with the
 * following limitations:
 * - one loop index (this means nested loops are not required to function according to the standard!!!)
 * - index has type int or float
 * - for statement must have the form:
 *     for (type_specifier identifier = constant_expression ;
 *          loop_index op constant_expression ;
 *          loop_expression )
 *       statement
 *    where op is > >= < <= == or !=, and loop_expression is of the form
 *       loop_index++, loop_index--, loop_index += constant_expression, loop_index -= constant_expression
 *	 https://www.khronos.org/webgl/public-mailing-list/archives/1012/msg00063.php
 *
 * Also there are some limitations to texture lookup using implicit derivatives, there are conditions
 * where behaviour is undefined.
 *	 http://gamedev.stackexchange.com/questions/32543/glsl-if-else-statement-unexpected-behaviour
 *
 * @return glsl code which will sample from the lut
 */
const std::string getSampleLutImplementation(int numberOfUploadedTextures)
{
	std::string lut_sampler_code;

	for(int i=0; i<numberOfUploadedTextures; ++i)
	{
		std::string j = QString::number(i).toStdString();
		std::string code = "	if(texture_index == "+j+")\n"
				   "		rgba_lut_values["+j+"] = texture("+FS_Uniform_1DTexture_LUT+"["+j+"], red_value);\n";
		lut_sampler_code += code;
	}

	return lut_sampler_code;
}

const std::string getFS(int numberOfUploadedTextures)
{
	std::string fs_shader_text;

	if(numberOfUploadedTextures != 0)
	{
		std::string number_of_textures = QString::number(numberOfUploadedTextures).toStdString();
		fs_shader_text =
		  "//VTK::System::Dec\n"
		  "//VTK::Output::Dec\n\n"
		  ""
		  "in vec3 normalVCVSOutput;"
		  "in vec4 vertexVCVSOutput;"
		  ""
		  "//CX: adding custom fragment shader\n"
		  "const int number_of_textures = "+number_of_textures+";\n"
		  "in vec3 "+VS_Out_Vec3_TextureCoordinate+"[number_of_textures];\n"
		  "uniform sampler3D "+FS_Uniform_3DTexture_Volume+"[number_of_textures];\n"
		  "uniform sampler1D "+FS_Uniform_1DTexture_LUT+"[number_of_textures];\n"
		  "uniform float "+FS_Uniform_Window+"[number_of_textures];\n"
		  "uniform float "+FS_Uniform_Level+"[number_of_textures];\n"
		  "uniform float "+FS_Uniform_LLR+"[number_of_textures];\n"
		  "uniform float "+FS_Uniform_Alpha+"[number_of_textures];\n"
		  "out vec4 "+FS_Out_Vec4_Color+";\n"
		  ""
		  "const vec3 bounds_lo = vec3(0.0,0.0,0.0);"
		  "const vec3 bounds_hi = vec3(1.0,1.0,1.0);"
		  ""
		  "bool textureCoordinateIsOutsideTexture(in int texture_index)\n"
		  "{\n"
		  "	vec3 texture_coordinate = "+VS_Out_Vec3_TextureCoordinate+"[texture_index];\n"
		  "	return any(lessThan(texture_coordinate, bounds_lo)) || any(greaterThan(texture_coordinate, bounds_hi));\n"
		  "}\n"
		  ""
		  "float windowLevel(in float x, in float window_, in float level_)\n"
		  "{\n"
		  "	return (x-level_)/window_ + 0.5;\n"
		  "}\n"
		  ""
		  "vec4 sampleLut(in int texture_index, in float red_value)\n"
		  "{\n"
		  "	vec4 rgba_lut_values[number_of_textures];\n"
		  +getSampleLutImplementation(numberOfUploadedTextures)+
		  "	return rgba_lut_values[texture_index];\n"
		  "}\n"
		  ""
		  "vec4 mergeTexture_GL_RED(in vec4 base_color,in int texture_index)\n"
		  "{\n"
		  "	//Ignore drawing outside texture\n"
		  "	bool outside = textureCoordinateIsOutsideTexture(texture_index);\n"
		  "	if(outside)\n"
		  "	{\n"
		  "		return base_color;\n"
		  "	}\n"
		  ""
		  "	//Sampling from GL_RED 3D texture \n"
		  "	vec4 rgba_texture_value = texture("+FS_Uniform_3DTexture_Volume+"[texture_index], "+VS_Out_Vec3_TextureCoordinate+"[texture_index]);\n"
		  "	float red_value = rgba_texture_value.r;\n"
		  ""
		  "	float llr = "+FS_Uniform_LLR+"[texture_index];\n"
		  "	if(red_value < llr)\n"
		  "	{\n"
		  "		return base_color;\n"
		  "	}\n"
		  ""
		  "	red_value = windowLevel(red_value, "+FS_Uniform_Window+"[texture_index], "+FS_Uniform_Level+"[texture_index]);\n"
		  "	red_value = clamp(red_value, 0.0, 1.0);\n"
		  ""
		  "	vec4 color_rbga = sampleLut(texture_index, red_value);\n"
		  "	color_rbga.a = "+FS_Uniform_Alpha+"[texture_index];\n"
		  "	color_rbga =  mix(base_color, color_rbga, "+FS_Uniform_Alpha+"[texture_index]);\n"
		  ""
		  "	return color_rbga;\n"
		  "}\n"
		  ""
		  "vec4 mergeTexture(in vec4 base_color, in int texture_index)\n"
		  "{\n"
		  "	return mergeTexture_GL_RED(base_color, texture_index);\n"
		  "}\n"
		  ""
		  "void main ()\n"
		  "{\n"
		  ""
		  "	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);\n"
		  "	for(int i=0; i<number_of_textures; i++)\n"
		  "	{\n"
		  "		color = mergeTexture(color, i);\n"
		  "	}\n"
		  ""
		  "	// if input variable (in VS or FS) does not affect any used results it will be optimized out by some glsl compilers"
		  "	// hack to make sure normalMC (used to calculate normalVCVSOutput in VS) is not optimized out\n"
		  "	color += vec4(normalVCVSOutput, 0.0);\n "
		  "	color -= vec4(normalVCVSOutput, 0.0);\n"
		  ""
		  "	"+FS_Out_Vec4_Color+" = color;\n"
		  "}"
		  ;
	}
	else
	{
		//If there are no textures present we do not need a fragmentshader
		fs_shader_text = "";
	}

	return fs_shader_text;
}

} //namespace cx

