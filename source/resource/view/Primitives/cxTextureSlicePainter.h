/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

#if !defined(CX_VTK_OPENGL2) && !defined(WIN32)

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


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* CXTEXTURESLICEPAINTER_H_ */
