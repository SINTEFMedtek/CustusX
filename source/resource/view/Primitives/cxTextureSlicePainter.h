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


/*
 * vmTextureSlicePainter.h
 *
 *  Created on: Oct 13, 2009
 *      Author: petterw
 */

#ifndef CXTEXTURESLICEPAINTER_H_
#define CXTEXTURESLICEPAINTER_H_

#include "cxResourceVisualizationExport.h"

#include <boost/shared_ptr.hpp>
#include <QString>
#include <vector>
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxConfig.h"

#ifndef CX_VTK_OPENGL2
#include <vtkOpenGLRepresentationPainter.h>
#endif

class vtkOpenGLRenderWindow;
class vtkUniformVariables;
//---------------------------------------------------------
namespace cx
{

//#ifndef WIN32
//#ifndef CX_VTK_OPENGL2
#if !defined(CX_VTK_OPENGL2) || !defined(WIN32)

class cxResourceVisualization_EXPORT SingleVolumePainterHelper
{
	GPUImageDataBufferPtr mVolumeBuffer;
	GPUImageLutBufferPtr mLutBuffer;
	int mIndex;
	float mWindow;
	float mLevel;
	float mLLR;
	float mAlpha;

public:
	explicit SingleVolumePainterHelper(int index);
	SingleVolumePainterHelper();
	~SingleVolumePainterHelper();
	void SetBuffer(GPUImageDataBufferPtr buffer);
	void SetBuffer(GPUImageLutBufferPtr buffer);
	void SetColorAttribute(float window, float level, float llr,float alpha);
	void initializeRendering();
	void setUniformiArray(vtkUniformVariables* uniforms, QString name, int val);
	void setUniformfArray(vtkUniformVariables* uniforms, QString name, float val);
	void eachRenderInternal(vtkSmartPointer<vtkShaderProgram2> shader);
};


/**
 * \brief Helper class for GPU rendering of slices.
 *
 * \sa Texture3DSlicerProxyImpl
 *
 *
 * \ingroup cx_resource_view
 */
class cxResourceVisualization_EXPORT TextureSlicePainter : public vtkOpenGLRepresentationPainter
{
public:
	static TextureSlicePainter* New();
	vtkTypeMacro(TextureSlicePainter, vtkOpenGLRepresentationPainter);

	virtual void ReleaseGraphicsResources(vtkWindow *);
	void PrintSelf(ostream& os, vtkIndent indent);

	void setShaderPath(QString path);
	void SetColorAttribute(int index, float window, float level, float llr, float alpha);
	void SetVolumeBuffer(int index, GPUImageDataBufferPtr buffer);
	void SetLutBuffer(int index, GPUImageLutBufferPtr buffer);
	void releaseGraphicsResources(int index);

	static bool LoadRequiredExtensions(vtkOpenGLExtensionManager* mgr);

protected:
	TextureSlicePainter();
	virtual ~TextureSlicePainter();
	virtual void PrepareForRendering(vtkRenderer*, vtkActor*);
	void RenderInternal(vtkRenderer* renderer, vtkActor* actor, unsigned long typeflags, bool forceCompileOnly);
	bool CanRender(vtkRenderer*, vtkActor*);
	static bool LoadRequiredExtension(vtkOpenGLExtensionManager* mgr, QString id);
	void ClearGraphicsResources();

private:
	QString loadShaderFile();
	void buildProgram(QString shaderSource, vtkOpenGLRenderWindow* renderWindow);
	SingleVolumePainterHelper& safeIndex(int index);
	QString replaceShaderSourceMacros(QString shaderSource);

	bool hasLoadedExtensions;
	QString mShaderPath;

	vtkWeakPointer<vtkRenderWindow> LastContext;
	vtkSmartPointer<vtkShaderProgram2> Shader;
	std::vector<SingleVolumePainterHelper> mElement;
};

#endif //CX_VTK_OPENGL2
//#endif // WIN32


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* CXTEXTURESLICEPAINTER_H_ */
