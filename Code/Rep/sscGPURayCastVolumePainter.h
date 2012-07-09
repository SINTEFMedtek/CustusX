// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2012- SINTEF Medical Technology
// Copyright (C) 2012- Sonowand AS
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

#ifndef SSCGPURAYCASTVOLUMEPAINTER_H_
#define SSCGPURAYCASTVOLUMEPAINTER_H_

#include <boost/shared_ptr.hpp>
#include <vtkOpenGLRepresentationPainter.h>
#include <QString>

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"
#include "sscView.h"
//---------------------------------------------------------
class TestGPURayCaster;

namespace ssc
{

#ifndef WIN32


/**
 * \brief Helper class for GPU raycasting rendering of multiple volumes.
 *
 * \sa GPURayCastVolumeRep
 *
 *
 * \ingroup sscRep3D
 */
class GPURayCastVolumePainter : public vtkOpenGLRepresentationPainter
{
public:
	static GPURayCastVolumePainter* New();
	vtkTypeRevisionMacro(GPURayCastVolumePainter, vtkOpenGLRepresentationPainter);

	virtual void ReleaseGraphicsResources(vtkWindow *);
	void PrintSelf(ostream& os, vtkIndent indent);

	void setShaderFolder(QString folder);
	void SetColorAttribute(int index, float window, float level, float llr, float alpha);
	void SetVolumeBuffer(int index, ssc::GPUImageDataBufferPtr buffer, double maxValue);
	void SetLutBuffer(int index, ssc::GPUImageLutBufferPtr buffer);
	void set_nMr(int index, ssc::Transform3D nMr);
	void setClipper(SlicePlaneClipperPtr clipper);
	void setClipVolume(int index, bool clip);
	void setStepSize(double stepsize);
	void enableImagePlaneDownsampling(int maxPixels);
	void disableImagePlaneDownsampling();
	/**
	 * Maximum number of volumes that can be rendered simultaneously
	 */
	static const unsigned maxVolumes;

protected:
	GPURayCastVolumePainter();
	virtual ~GPURayCastVolumePainter();
	virtual void PrepareForRendering(vtkRenderer*, vtkActor*);
	void RenderInternal(vtkRenderer* renderer, vtkActor* actor, unsigned long typeflags, bool forceCompileOnly);

	bool CanRender(vtkRenderer*, vtkActor*);
	bool LoadRequiredExtensions(vtkOpenGLExtensionManager* mgr);
	bool LoadRequiredExtension(vtkOpenGLExtensionManager* mgr, QString id);
	QString loadShaderFile(QString shaderFile);
	void createBuffers();

	class vtkInternals;
	vtkInternals* mInternals;
	QString mVertexShaderFile;
	QString mFragmentShaderFile;
	QString mUSVertexShaderFile;
	QString mUSFragmentShaderFile;
	QSize mLastRenderSize;
	float mWidth;
	float mHeight;
	unsigned int mDepthBuffer;
	unsigned int mBackgroundBuffer;
	SlicePlaneClipperPtr mClipper;
	float mStepSize;
	int mRenderMode;
	unsigned int mFBO;
	unsigned int mDSColorBuffer;
	unsigned int mDSDepthBuffer;
	int mDownsampleWidth;
	int mDownsampleHeight;
	int mDownsamplePixels;
	vtkSmartPointer<vtkShaderProgram2> mUpscaleShader;
	bool mShouldResample;
	bool mResample;
	friend class ::TestGPURayCaster;

private:
	bool hasLoadedExtensions;
};

#endif // WIN32


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* SSCGPURAYCASTVOLUMEPAINTER_H_ */
