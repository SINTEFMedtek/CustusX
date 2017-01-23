/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXSINGLEVOLUMEPAINTERHELPER_H
#define CXSINGLEVOLUMEPAINTERHELPER_H

#include "cxResourceVisualizationExport.h"
#include "cxForwardDeclarations.h"
#include <vtkCommand.h>

#include "cxLogger.h"


class vtkOpenGLHelper;
class vtkShaderProgram;
class vtkOpenGLVertexArrayObject;

typedef vtkSmartPointer<class vtkTextureObject> vtkTextureObjectPtr;
typedef vtkSmartPointer<class vtkOpenGLBufferObject> vtkOpenGLBufferObjectPtr;

namespace cx
{
typedef vtkSmartPointer<class ShaderCallback> ShaderCallbackPtr;

/**
 * @brief The ShaderCallback class is used to update information sent to our
 * custom OpenGL shaders.
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
class cxResourceVisualization_EXPORT ShaderCallback : public vtkCommand
{

public:
	static const std::string VS_In_Vec3_TextureCoordinate;
	static const std::string VS_Out_Vec3_TextureCoordinate;
	static const std::string FS_In_Vec3_TextureCoordinate;
	static const std::string FS_Uniform_3DTexture_Volume;
	static const std::string FS_Uniform_1DTexture_LUT;
	static const std::string FS_Uniform_Window;
	static const std::string FS_Uniform_Level;
	static const std::string FS_Uniform_LLR;
	static const std::string FS_Uniform_Alpha;
	static const std::string FS_Out_Vec4_Color;

	const std::string getVSReplacement_dec(std::string vtk_dec) const;
	const std::string getVSReplacement_impl(std::string vtk_impl) const;
	const std::string getFS() const;

	//static const int Const_Int_NumberOfTextures; //TODO remove, use getShaderItemSize

	/**
	 * Conventient structure that groups relevant information for our custom OpenGL shaders on a per view basis.
	 */
	struct ShaderItem
	{
		QString mTextureUid;
		vtkTextureObjectPtr mTexture;

		QString mTextureCoordinatesUid;
		vtkOpenGLBufferObjectPtr mTextureCoordinates;

		QString mLUTUid;
		vtkTextureObjectPtr mLUT;

		//int mLutSize; //basically: used to check if lut should be applied, also a check if it is RED or RGB(A)
		float mWindow;
		float mLevel;
		float mLLR; //low level reject
		float mAlpha;
	};
	typedef boost::shared_ptr<ShaderItem> ShaderItemPtr;

	static ShaderCallback *New();
	ShaderCallback();
	virtual ~ShaderCallback();

	virtual void Execute(vtkObject *, unsigned long eventId, void*cbo);

	ShaderItemPtr getShaderItem(QString image_uid) const;
	int getNumberOfUploadedTextures() const;

	//SharedOpenGLContextPtr mSharedOpenGLContext;
	//vtkRenderWindowPtr mCurrentContext;
	std::vector<ShaderItemPtr> mShaderItems;

private:
	void addArrayToAttributeArray(vtkShaderProgram *program, vtkOpenGLBufferObjectPtr buffer, std::string name, int vector_index);
	void addToAttributeArray(vtkOpenGLVertexArrayObject *vao, vtkShaderProgram *program, vtkOpenGLBufferObjectPtr buffer, std::string name);
	void addUniformiArray(vtkShaderProgram *program, std::string name, int value);
	void addUniformfArray(vtkShaderProgram *program, std::string name, float value);
	void bindFSOutputVariable(vtkShaderProgram *program);

	std::string getVectorNameFromName(std::string name, int index_of_vector) const;

	void printDebugInfo(vtkOpenGLHelper *OpenGLHelper);

	//----- DELETE???-------
public:
	//ShaderCallback(int index);

	//GPUImageDataBufferPtr mVolumeBuffer;
	//GPUImageLutBufferPtr mLutBuffer;
	//int mIndex;
	//float mWindow;
	//float mLevel;
	//float mLLR;
	//float mAlpha;
	//	vtkRenderer *Renderer;

public:
	//	explicit SingleVolumePainterHelper(int index);
	//	SingleVolumePainterHelper();
	//	~SingleVolumePainterHelper();
	//void SetBuffer(GPUImageDataBufferPtr buffer);
	//void SetBuffer(GPUImageLutBufferPtr buffer);
	//void SetColorAttribute(float window, float level, float llr,float alpha);
	//void initializeRendering();
	//void setUniformiArray(vtkOpenGLHelper *cellBO, QString name, int val);
	//void setUniformfArray(vtkOpenGLHelper *cellBO, QString name, float val);
	//	void eachRenderInternal(vtkSmartPointer<vtkShaderProgram2> shader);
	//void eachRenderInternal(vtkOpenGLHelper *cellBO);

private:
	//void init(int index);
	//	void uploadTextureCoordinate(vtkOpenGLHelper *cellBO);
	//----- DELETE???-------
	std::string getSampleLutImplementation() const;
};

}//cx

#endif // CXSINGLEVOLUMEPAINTERHELPER_H
