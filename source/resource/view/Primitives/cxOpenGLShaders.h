#ifndef OPENGLSHADERS_H
#define OPENGLSHADERS_H

#include <string>

namespace cx
{

/**
 * Set of convenient functions and definitions that creates CustusX custom fragment and vertex shaders for OpenGL
 * which enables 2D overlay.
 *
 * CustusX uses VTK shaders from OpenGLPolydataMapper and then modifies them to display several volumes in 2D. Textures are read from a
 * shared context which is a hack in VTK.
 *
 * Definitions:
 * VS = Vertextshader
 * FS = Fragmentshader
 * In = attribute (in to the VS or FS)
 * Out = varying (out from the VS or FS)
 * Uniform = global value across all fragments or vertices
 * Shader - refering to our custom OpenGL vertex and fragment shaders, there is one set of shaders per renderwindow (view)
 * Texture - there is one texture per image
 * TextureCoordinates - there is one texture coordinate per view per image
 * LUT - look up table, there is one LUT per image (?)
*/

static const std::string VS_In_Vec3_TextureCoordinate = "cx_vs_in_texture_coordinates";
static const std::string VS_Out_Vec3_TextureCoordinate = "cx_vs_out_texture_coordinates";
static const std::string FS_In_Vec3_TextureCoordinate = "cx_fs_in_texture_coordinates";
static const std::string FS_Uniform_3DTexture_Volume = "cx_fs_uniform_3Dtexture";
static const std::string FS_Uniform_1DTexture_LUT = "cx_fs_uniform_1Dtexture";
static const std::string FS_Uniform_Window = "cx_fs_uniform_window";
static const std::string FS_Uniform_Level = "cx_fs_uniform_level";
static const std::string FS_Uniform_LLR = "cx_fs_uniform_llr";
static const std::string FS_Uniform_Alpha = "cx_fs_uniform_alpha";
static const std::string FS_Out_Vec4_Color = "cx_fs_out_color";

const std::string getVSReplacement_dec(std::string vtk_dec, int numberOfUploadedTextures);
const std::string getVSReplacement_impl(std::string vtk_impl, int numberOfUploadedTextures);
const std::string getFS(int numberOfUploadedTextures);
const std::string getSampleLutImplementation(int numberOfUploadedTextures);

} //namespace cx

#endif // OPENGLSHADERS_H
