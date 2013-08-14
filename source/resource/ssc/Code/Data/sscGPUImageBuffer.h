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

#ifndef SSCGPUIMAGEBUFFER_H_
#define SSCGPUIMAGEBUFFER_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>
#include <list>
#include "vtkForwardDeclarations.h"

namespace ssc
{

/**
 * \addtogroup sscData
 * \{
 */

/**\brief Helper class for sharing GPU memory over several Views (GL contexts).
 *
 * This one stores 3D volume texture in the form of a vtkImageDataPtr
 *
 * \ingroup sscData
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
	virtual void updateTexture() = 0;
	/**Activate and bind the volume and lut buffers inside the texture units
	 * GL_TEXTURE<2X> and GL_TEXTURE<2X+1>.
	 * Use during RenderInternal()
	 */
	virtual void bind(int textureUnitIndex) = 0;
	virtual int getMemorySize() = 0;
};

typedef boost::shared_ptr<GPUImageDataBuffer> GPUImageDataBufferPtr;
typedef boost::weak_ptr<GPUImageDataBuffer> GPUImageDataBufferWeakPtr;

GPUImageDataBufferPtr createGPUImageDataBuffer(vtkImageDataPtr volume);

/**\brief Helper class for sharing GPU memory over several Views (GL contexts).
 *
 * This one stores lut in the form of a vtkUnsignedCharArrayPtr
 *
 * \ingroup sscData
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

/**\brief Repository for GPU buffers
 *
 * Use this repository for avoiding multiple allocations of buffer data. *
 *
 * \ingroup sscData
 */
class GPUImageBufferRepository
{
public:
	static GPUImageBufferRepository* getInstance();
	static void shutdown();

	ssc::GPUImageDataBufferPtr getGPUImageDataBuffer(vtkImageDataPtr volume);
	ssc::GPUImageLutBufferPtr getGPUImageLutBuffer(vtkUnsignedCharArrayPtr lut);
	int getMemoryUsage(int *textures);
	/**
	 * Free all resources in the repository, for instance on session change
	 */
	void clear();
private:
	GPUImageBufferRepository();
	~GPUImageBufferRepository();
	class GPUImageBufferRepositoryInternal* mInternal;
	static GPUImageBufferRepository* mInstance;
	void tearDown();
};

/**
 * \}
 */

}

#endif /*SSCGPUIMAGEBUFFER_H_*/
