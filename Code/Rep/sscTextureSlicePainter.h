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

#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkOpenGLHardwareSupport > vtkOpenGLHardwareSupportPtr;
typedef vtkSmartPointer<class vtkShaderProgram2 > vtkShaderProgram2Ptr;
typedef vtkSmartPointer<class vtkGLSLShaderDeviceAdapter2 > vtkGLSLShaderDeviceAdapter2Ptr;
typedef vtkSmartPointer<class vtkShader2 > vtkShader2Ptr;
class vtkOpenGLExtensionManager;

//---------------------------------------------------------
namespace ssc
{
//---------------------------------------------------------
typedef vtkSmartPointer<class TextureSlicePainter > TextureSlicePainterPtr;
// forward declarations
typedef boost::shared_ptr<class GPUImageDataBuffer> GPUImageDataBufferPtr;
typedef boost::shared_ptr<class GPUImageLutBuffer> GPUImageLutBufferPtr;


/**Helper class for GPU rendering of slices.
 * Used by Texture3DSlicerRep.
 */
class TextureSlicePainter : public vtkOpenGLRepresentationPainter
{
public:
	static TextureSlicePainter* New();
	vtkTypeRevisionMacro(TextureSlicePainter, vtkOpenGLRepresentationPainter);

	virtual void ReleaseGraphicsResources(vtkWindow *);
	void PrintSelf(ostream& os, vtkIndent indent);

	//void SetColorMap(int index, vtkUnsignedCharArrayPtr table);
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
	bool LoadRequiredExtension(vtkOpenGLExtensionManager* mgr, std::string id);

	class vtkInternals;
	vtkInternals* mInternals;
	std::string mSource;
};

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /* SSCTEXTURESLICEPAINTER_H_ */
