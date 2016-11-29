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

class cxResourceVisualization_EXPORT ShaderCallback : public vtkCommand
{

public:
	//VS = Vertextshader
	//FS = Fragmentshader
	//In = attribute (in VS)
	//Out = varying (from VS)
	static const std::string VS_In_Vec3_TextureCoordinate;
	static const std::string VS_Out_Vec3_TextureCoordinate;
	static const std::string FS_In_Vec3_TextureCoordinate;
	static const std::string FS_Uniform_3DTexture;
	static const std::string FS_Out_Vec4_Color;
	static const int Const_Int_NumberOfTextures;

	struct ShaderItem
	{
		QString mImageUid;
		vtkTextureObjectPtr mTexture;
	};
	typedef boost::shared_ptr<ShaderItem> ShaderItemPtr;

	static ShaderCallback *New();
	ShaderCallback();

	virtual void Execute(vtkObject *, unsigned long eventId, void*cbo);

	SharedOpenGLContextPtr mSharedOpenGLContext;
	vtkRenderWindowPtr mCurrentContext;
	std::vector<ShaderItemPtr> mShaderItems;
	vtkOpenGLBufferObjectPtr mTextureCoordinates;

private:
	void addToAttributeArray(vtkOpenGLVertexArrayObject *vao, vtkShaderProgram *program, vtkOpenGLBufferObjectPtr buffer, std::string name);
	void addUniform(vtkShaderProgram *program, std::string name, int value);
	void bindFSOutputVariable(vtkShaderProgram *program);

	std::string generateVSAttributeTextureCoordinateVectorName(int index_of_vector) const;
	std::string generateFSUniformTextureVectorName(int index_of_vector) const;
	std::string getVectorNameFromName(std::string name, int index_of_vector) const;

	void printDebugInfo(vtkOpenGLHelper *OpenGLHelper);

	//----- DELETE???-------
public:
	ShaderCallback(int index);
	virtual ~ShaderCallback();

	GPUImageDataBufferPtr mVolumeBuffer;
	GPUImageLutBufferPtr mLutBuffer;
	int mIndex;
	float mWindow;
	float mLevel;
	float mLLR;
	float mAlpha;
	//	vtkRenderer *Renderer;

public:
	//	explicit SingleVolumePainterHelper(int index);
	//	SingleVolumePainterHelper();
	//	~SingleVolumePainterHelper();
	void SetBuffer(GPUImageDataBufferPtr buffer);
	void SetBuffer(GPUImageLutBufferPtr buffer);
	void SetColorAttribute(float window, float level, float llr,float alpha);
	void initializeRendering();
	void setUniformiArray(vtkOpenGLHelper *cellBO, QString name, int val);
	void setUniformfArray(vtkOpenGLHelper *cellBO, QString name, float val);
	//	void eachRenderInternal(vtkSmartPointer<vtkShaderProgram2> shader);
	//void eachRenderInternal(vtkOpenGLHelper *cellBO);

private:
	void init(int index);
	//	void uploadTextureCoordinate(vtkOpenGLHelper *cellBO);
	//----- DELETE???-------
};

}//cx

#endif // CXSINGLEVOLUMEPAINTERHELPER_H
