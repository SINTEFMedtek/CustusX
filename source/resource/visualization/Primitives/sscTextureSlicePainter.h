// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * vmTextureSlicePainter.h
 *
 *  Created on: Oct 13, 2009
 *      Author: petterw
 */

#ifndef SSCTEXTURESLICEPAINTER_H_
#define SSCTEXTURESLICEPAINTER_H_

#include <boost/shared_ptr.hpp>
#include <vtkOpenGLRepresentationPainter.h>
#include <QString>
#include <vector>
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
class vtkOpenGLRenderWindow;
class vtkUniformVariables;
//---------------------------------------------------------
namespace cx
{

#ifndef WIN32

class SingleVolumePainterHelper
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
 * \ingroup cx_resource_visualization
 */
class TextureSlicePainter : public vtkOpenGLRepresentationPainter
{
public:
	static TextureSlicePainter* New();
	vtkTypeRevisionMacro(TextureSlicePainter, vtkOpenGLRepresentationPainter);

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

#endif // WIN32


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* SSCTEXTURESLICEPAINTER_H_ */
