#ifndef SSCGPUIMAGEBUFFER_H_
#define SSCGPUIMAGEBUFFER_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkUnsignedCharArray> vtkUnsignedCharArrayPtr;

namespace ssc
{

/**Helper class for sharing GPU memory over several Views (GL contexts).
 * 
 * 
 */
class GPUImageBuffer
{
public:
	virtual ~GPUImageBuffer() {}
	/**Allocate resources for the lookup table and the volume on the GPU.
	 * Prerequisite: SetImage and SetcolorTable has been called.
	 * 
	 * Call this from inside a PrepareForRendering() methods in vtk.
	 */
	virtual void allocate() = 0;
	/**Activate and bind the volume and lut buffers inside the texture units 
	 * GL_TEXTURE<2X> and GL_TEXTURE<2X+1>. 
	 * Use during RenderInternal()
	 */
	virtual void bind(int textureUnitIndex) = 0;	
};
typedef boost::shared_ptr<GPUImageBuffer> GPUImageBufferPtr;
typedef boost::weak_ptr<GPUImageBuffer> GPUImageBufferWeakPtr;

GPUImageBufferPtr createGPUImageBuffer(vtkImageDataPtr volume, vtkUnsignedCharArrayPtr lut);

}

#endif /*SSCGPUIMAGEBUFFER_H_*/
