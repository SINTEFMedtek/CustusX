#ifndef SSCGPUIMAGEBUFFER_H_
#define SSCGPUIMAGEBUFFER_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkUnsignedCharArray> vtkUnsignedCharArrayPtr;

namespace ssc
{

/**Helper class for sharing GPU memory over several Views (GL contexts).
 *
 * This one stores 3D volume texture in the form of a vtkImageDataPtr
 */
class GPUImageDataBuffer
{
public:
	virtual ~GPUImageDataBuffer() {}
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

typedef boost::shared_ptr<GPUImageDataBuffer> GPUImageDataBufferPtr;
typedef boost::weak_ptr<GPUImageDataBuffer> GPUImageDataBufferWeakPtr;

GPUImageDataBufferPtr createGPUImageDataBuffer(vtkImageDataPtr volume);

/**Helper class for sharing GPU memory over several Views (GL contexts).
 * This one stores lut in the form of a vtkUnsignedCharArrayPtr
 *
 */
class GPUImageLutBuffer
{
public:
	virtual ~GPUImageLutBuffer() {}
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
	/**Size of lut. Used by fragment shader.
	 */
	virtual int getLutSize() const = 0;
};

typedef boost::shared_ptr<GPUImageLutBuffer> GPUImageLutBufferPtr;
typedef boost::weak_ptr<GPUImageLutBuffer> GPUImageLutBufferWeakPtr;

GPUImageDataBufferPtr createGPUImageDataBuffer(vtkImageDataPtr volume);
GPUImageLutBufferPtr createGPUImageLutBuffer(vtkUnsignedCharArrayPtr lut);

class GPUImageBufferRepository
{
public:
	GPUImageBufferRepository();
	static GPUImageBufferRepository* getInstance();
	ssc::GPUImageDataBufferPtr getGPUImageDataBuffer(vtkImageDataPtr volume);
	ssc::GPUImageLutBufferPtr getGPUImageLutBuffer(vtkUnsignedCharArrayPtr lut);
private:
	unsigned mMaxVolumes;
	unsigned mMaxLuts;
	std::map<vtkImageDataPtr, ssc::GPUImageDataBufferPtr> mVolumes;
	std::map<vtkUnsignedCharArrayPtr, ssc::GPUImageLutBufferPtr> mLuts;
	static GPUImageBufferRepository* mInstance;
};

}

#endif /*SSCGPUIMAGEBUFFER_H_*/
