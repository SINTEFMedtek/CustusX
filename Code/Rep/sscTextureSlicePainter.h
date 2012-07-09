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

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

//---------------------------------------------------------
namespace ssc
{

#ifndef WIN32


/**
 * \brief Helper class for GPU rendering of slices.
 *
 * \sa Texture3DSlicerProxyImpl
 *
 *
 * \ingroup sscProxy
 */
class TextureSlicePainter : public vtkOpenGLRepresentationPainter
{
public:
	static TextureSlicePainter* New();
	vtkTypeRevisionMacro(TextureSlicePainter, vtkOpenGLRepresentationPainter);

	virtual void ReleaseGraphicsResources(vtkWindow *);
	void PrintSelf(ostream& os, vtkIndent indent);

	void setShaderFile(QString shaderFile);
	void SetColorAttribute(int index, float window, float level, float llr, float alpha);
	void SetVolumeBuffer(int index, ssc::GPUImageDataBufferPtr buffer);
	void SetLutBuffer(int index, ssc::GPUImageLutBufferPtr buffer);
	void releaseGraphicsResources(int index);

protected:
	TextureSlicePainter();
	virtual ~TextureSlicePainter();
	virtual void PrepareForRendering(vtkRenderer*, vtkActor*);
	void RenderInternal(vtkRenderer* renderer, vtkActor* actor, unsigned long typeflags, bool forceCompileOnly);

	bool CanRender(vtkRenderer*, vtkActor*);
	bool LoadRequiredExtensions(vtkOpenGLExtensionManager* mgr);
	bool LoadRequiredExtension(vtkOpenGLExtensionManager* mgr, QString id);
	QString loadShaderFile(QString shaderFile);

	class vtkInternals;
	vtkInternals* mInternals;
	QString mSource;
	QString mShaderFile;

private:
	bool hasLoadedExtensions;
};

#endif // WIN32


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* SSCTEXTURESLICEPAINTER_H_ */
