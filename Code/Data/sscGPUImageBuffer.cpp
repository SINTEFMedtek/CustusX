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

#include "sscGPUImageBuffer.h"

#include <QString>

#ifndef WIN32
#define GL_GLEXT_PROTOTYPES
#include <vtkgl.h>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <GL/glext.h>
#endif

#include <vector>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnsignedShortArray.h>
#include <boost/cstdint.hpp>

#include "sscGLHelpers.h"

namespace ssc
{

/**Helper class for sharing volume and lut buffers over several renderings
 */
class GPUImageDataBufferImpl : public GPUImageDataBuffer
{
public:
	GLuint textureId;
	vtkImageDataPtr mTexture;
	bool mAllocated;
	uint64_t mMTime;
	int mMemorySize;

	GPUImageDataBufferImpl()
	{
		mAllocated = false;
		mMTime = 0;
		textureId = 0;
		mMemorySize = 0;
	}
	virtual ~GPUImageDataBufferImpl()
	{
		release();
	}

	virtual int getMemorySize()
	{
		return mMemorySize;
	}

	virtual void SetImage (vtkImageDataPtr texture)
	{
		if (!texture)
		{
			std::cout << "error: bad buffer initialization: null image" << std::endl;
		}
		mTexture = texture;
	}

	/**Allocate resources for the lookup table and the volume on the GPU.
	 * Prerequisite: SetImage and SetcolorTable has been called.
	 *
	 * Call this from inside a PrepareForRendering() methods in vtk.
	 */
	virtual void allocate()
	{
		if (mAllocated) // do this only once.
		{
			return;
		}
		if (!mTexture)
		{
			std::cout << "error: bad volume buffer initialization" << std::endl;
			return;
		}

		vtkgl::ActiveTexture(GL_TEXTURE7);

		glEnable( vtkgl::TEXTURE_3D );
		glGenTextures(1, &textureId);
		glDisable(GL_TEXTURE_3D);
		updateTexture();
		mAllocated = true;
	}
	virtual void updateTexture()
	{
		if (mMTime == mTexture->GetMTime())
		{
			return;
		}
		mMTime = mTexture->GetMTime();
		//vtkgl::ActiveTexture(getGLTextureForVolume(textureUnitIndex)); //TODO is this OK?
		GLenum size,internalType;
		boost::uint32_t dimx = mTexture ->GetDimensions( )[0];
		boost::uint32_t dimy = mTexture ->GetDimensions( )[1];
		boost::uint32_t dimz = mTexture ->GetDimensions( )[2];
		mMemorySize = dimx * dimy * dimz;

		glEnable( vtkgl::TEXTURE_3D );
		glBindTexture(vtkgl::TEXTURE_3D, textureId);
		report_gl_error();
		glTexParameteri( vtkgl::TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( vtkgl::TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		glTexParameteri( vtkgl::TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );
		glTexParameteri( vtkgl::TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( vtkgl::TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		switch (mTexture->GetScalarType())
		{
		case VTK_UNSIGNED_CHAR:
		{
			size = GL_UNSIGNED_BYTE;
			internalType = GL_LUMINANCE;
		}
			break; //8UI_EXT; break;
		case VTK_UNSIGNED_SHORT:
		{
			size = GL_UNSIGNED_SHORT;
			internalType = GL_LUMINANCE16;
			mMemorySize *= 2;
		}
			break; //16UI_EXT; break;
		default:
			size = 0;
			internalType = 0;
			std::cout << "Bit size not supported!" << std::endl;
			break;
		}

		if (mTexture->GetNumberOfScalarComponents()==1)
		{
			void* data = mTexture->GetPointData()->GetScalars()->GetVoidPointer(0);
			glTexImage3D(vtkgl::TEXTURE_3D, 0, internalType, dimx, dimy, dimz, 0, GL_LUMINANCE, size, data);
		}
		else if (mTexture->GetNumberOfScalarComponents()==3)
		{
			internalType = GL_RGB;
			void* data = mTexture->GetPointData()->GetScalars()->GetVoidPointer(0);
			glTexImage3D(vtkgl::TEXTURE_3D, 0, internalType, dimx, dimy, dimz, 0, GL_RGB, size, data);
			mMemorySize *= 3;
		}
		else
		{
			std::cout << "unsupported number of image components" << std::endl;
		}

		glDisable(GL_TEXTURE_3D);

		report_gl_error();
	}

	/**Activate and bind the volume and lut buffers inside the texture units
	 * GL_TEXTURE<2X> and GL_TEXTURE<2X+1>.
	 * Use during RenderInternal()
	 */
	virtual void bind(int textureUnitIndex)
	{
		if (!mAllocated)
		{
			std::cout << "error: called bind() on unallocated volume buffer" << std::endl;
			return;
		}
		vtkgl::ActiveTexture(getGLTextureForVolume(textureUnitIndex));
		glBindTexture(GL_TEXTURE_3D, textureId);
		report_gl_error();
	}

	virtual void release()
	{
		glDeleteTextures(1, &textureId);
	}

	int getGLTextureForVolume(int textureUnitIndex)
	{
		switch (textureUnitIndex)
		{
		case 0: return GL_TEXTURE0;
		case 1: return GL_TEXTURE2;
		case 2: return GL_TEXTURE4;
		case 3: return GL_TEXTURE6;
		case 4: return GL_TEXTURE8;
		default: return -1;
		}
	}
};


/**Helper class for sharing volume and lut buffers over several renderings
 */
class GPUImageLutBufferImpl : public GPUImageLutBuffer
{
public:
	GLuint lutBuffer;
	GLuint textureId;

	int mLutDataSize;
	int mLutSize;
	std::vector<float> mLut;
	bool mAllocated;
	int mMemorySize;

	GPUImageLutBufferImpl()
	{
		mAllocated = false;
		mLutSize = 0;
		mLutDataSize = 0;
		textureId = 0;
		mMemorySize = 0;
	}
	virtual ~GPUImageLutBufferImpl()
	{
		release();
	}

	//intput lookuptable is raw imported table
	virtual void SetColorMap(vtkUnsignedCharArrayPtr table)
	{
		mLutSize = table->GetNumberOfTuples();
		mLutDataSize = mLutSize * table->GetNumberOfComponents();
		mLut.resize(mLutDataSize);

		unsigned char* ptr = table->GetPointer(0);

		for (int i = 0; i < mLutDataSize; ++i)
		{
			mLut[i] = ((float) *ptr) / 255.0;
			++ptr;
		}
	}

	/**Allocate resources for the lookup table and the volume on the GPU.
	 * Prerequisite: SetImage and SetcolorTable has been called.
	 *
	 * Call this from inside a PrepareForRendering() methods in vtk.
	 */
	virtual void allocate()
	{
		if (mAllocated) // do this only once.
		{
			return;
		}
		if (mLut.empty())
		{
			std::cout << "error: bad lut buffer initialization" << std::endl;
			return;
		}

		glGenTextures(1, &textureId);

		/** upload color buffer **/
		vtkgl::ActiveTexture(GL_TEXTURE8);
		//vtkgl::ActiveTexture(getGLTextureForLut(textureUnitIndex)); //TODO is this OK?

		vtkgl::GenBuffersARB(1, &lutBuffer);
		vtkgl::BindBuffer(vtkgl::TEXTURE_BUFFER_EXT, lutBuffer);
		vtkgl::BufferData(vtkgl::TEXTURE_BUFFER_EXT, mLutDataSize * sizeof(float), &(*mLut.begin()), vtkgl::STATIC_DRAW);

		glBindTexture(vtkgl::TEXTURE_BUFFER_EXT, textureId);
		vtkgl::TexBufferEXT(vtkgl::TEXTURE_BUFFER_EXT, vtkgl::RGBA32F_ARB, lutBuffer);
		report_gl_error();

		glBindTexture(vtkgl::TEXTURE_BUFFER_EXT, 0);
		report_gl_error();

		mAllocated = true;
	}
	/**Activate and bind the volume and lut buffers inside the texture units
	 * GL_TEXTURE<2X> and GL_TEXTURE<2X+1>.
	 * Use during RenderInternal()
	 */
	virtual void bind(int textureUnitIndex)
	{
		if (!mAllocated)
		{
			std::cout << "error: called bind() on unallocated lut buffer" << std::endl;
			return;
		}
		vtkgl::ActiveTexture(getGLTextureForLut(textureUnitIndex));
		glBindTexture(vtkgl::TEXTURE_BUFFER_EXT, textureId);
		report_gl_error();
	}

	int getLutSize() const
	{
		return mLutSize;
	}

	virtual void release()
	{
		if (!mAllocated)
		{
			return;
		}
		glDeleteTextures(1, &textureId);
		vtkgl::DeleteBuffersARB(1,&lutBuffer);
	}

	int getGLTextureForLut(int textureUnitIndex)
	{
		switch (textureUnitIndex)
		{
		case 0: return GL_TEXTURE1;
		case 1: return GL_TEXTURE3;
		case 2: return GL_TEXTURE5;
		case 3: return GL_TEXTURE7;
		case 4: return GL_TEXTURE9;
		default: return -1;
		}
	}
};

GPUImageDataBufferPtr createGPUImageDataBuffer(vtkImageDataPtr volume)
{
	std::auto_ptr<GPUImageDataBufferImpl> retval(new GPUImageDataBufferImpl);
	retval->SetImage(volume);
	return GPUImageDataBufferPtr(retval.release());
}

GPUImageLutBufferPtr createGPUImageLutBuffer(vtkUnsignedCharArrayPtr lut)
{
	std::auto_ptr<GPUImageLutBufferImpl> retval(new GPUImageLutBufferImpl);
	retval->SetColorMap(lut);
	return GPUImageLutBufferPtr(retval.release());
}

template<class BUFFER, class DATA_PTR>
boost::shared_ptr<BUFFER> createGPUImageBuffer(DATA_PTR val);

template<>
boost::shared_ptr<GPUImageDataBuffer> createGPUImageBuffer<GPUImageDataBuffer>(vtkImageDataPtr val);
template<>
boost::shared_ptr<GPUImageLutBuffer> createGPUImageBuffer<GPUImageLutBuffer>(vtkUnsignedCharArrayPtr val);

template<>
boost::shared_ptr<GPUImageDataBuffer> createGPUImageBuffer<GPUImageDataBuffer,vtkImageDataPtr>(vtkImageDataPtr val)
{
	return createGPUImageDataBuffer(val);
}
template<>
boost::shared_ptr<GPUImageLutBuffer> createGPUImageBuffer<GPUImageLutBuffer>(vtkUnsignedCharArrayPtr val)
{
	return createGPUImageLutBuffer(val);
}


//---------------------------------------------------------
GPUImageBufferRepository* GPUImageBufferRepository::mInstance = NULL;
//---------------------------------------------------------

template<class DATA_PTR, class BUFFER>
class BufferQueue
{
public:
	typedef boost::shared_ptr<BUFFER> BufferPtr;
	typedef boost::weak_ptr<BUFFER> BufferWeakPtr;

	struct BufferStore
	{
		BufferStore(DATA_PTR data, BufferPtr buffer) : mData(data), mBuffer(buffer) {}
		DATA_PTR mData;
		BufferPtr mBuffer;
	};
public:

	BufferQueue() : mMaxBuffers(7)
	{

	}
	void setMaxBuffers(unsigned val)
	{
		mMaxBuffers = val;
	}
	void setName(const QString& name)
	{
		mName = name;
	}

	/** Get a GPU buffer for the input data. 
	 * Try to reuse existing memory if available.
	 * 
	 */
	BufferPtr get(DATA_PTR data)
	{
		// clear out deleted data
		for (typename BufferMap::iterator iter=mRemovedData.begin(); iter!=mRemovedData.end(); )
		{
			if (!iter->second.lock())
			{
				typename BufferMap::iterator temp = iter;
				++iter;
				mRemovedData.erase(temp);
			}
			else
			{
				++iter;
			}
		}

		// reclaim weak pointer to buffer if it exists.
		if (mRemovedData.count(data))
		{
			BufferPtr object = mRemovedData[data].lock();
			if (object)
			{
				mData.push_front(BufferStore(data, object));
				mRemovedData.erase(data);
			}
		}

		// move current data to front of buffer (i.e. increase importance)
		BufferPtr retval;
		for (typename std::list<BufferStore>::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
		{
			if (iter->mData==data)
			{
				retval = iter->mBuffer; // retrieve data
				mData.push_front(*iter); // push on queue front (most recent)
				mData.erase(iter); // erase from old position
				break;
			}
		}

		// create buffer if nonexistent
		if (!retval)
		{
			retval = createGPUImageBuffer<BUFFER>(data);
			mData.push_front(BufferStore(data, retval));
		}

		// reduce repository size if too large.
		while (mData.size()>mMaxBuffers)
		{
			mRemovedData[mData.back().mData] = mData.back().mBuffer;
			mData.pop_back();;
		}

		return retval;
	}
	int getMemoryUsage(int *textures)
	{
		int mem = 0;
		if (textures)
		{
			*textures = mData.size();
		}
		for (typename std::list<BufferStore>::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
		{
			mem += iter->mBuffer->getMemorySize();
		}
		return mem;
	}
private:
	typedef std::map<DATA_PTR, BufferWeakPtr> BufferMap;
	BufferMap mRemovedData; // those buffers that are removed but still might live outside of the repo.
	std::list<BufferStore> mData; // newest elems in front
	unsigned mMaxBuffers;
	QString mName; // debug
};

class GPUImageBufferRepositoryInternal
{
public:
	GPUImageBufferRepositoryInternal()
	{
		mVolumeBuffer.setName("Volume");
		mLutBuffer.setName("Lut");

		mVolumeBuffer.setMaxBuffers(12);
		mLutBuffer.setMaxBuffers(15);
	}
	BufferQueue<vtkImageDataPtr, ssc::GPUImageDataBuffer> mVolumeBuffer;
	BufferQueue<vtkUnsignedCharArrayPtr, ssc::GPUImageLutBuffer> mLutBuffer;
};

GPUImageBufferRepository::GPUImageBufferRepository()
{
	mInternal = new GPUImageBufferRepositoryInternal();
}

GPUImageBufferRepository* GPUImageBufferRepository::getInstance()
{
	if (!mInstance)
	{
		mInstance = new GPUImageBufferRepository();
	}
	return mInstance;
}

void GPUImageBufferRepository::shutdown()
{
	if(mInstance)
	{
		mInstance->tearDown();
		delete mInstance;
	}
	mInstance = NULL;

}

void GPUImageBufferRepository::tearDown()
{
	delete mInternal;
	mInternal = NULL;
}

int GPUImageBufferRepository::getMemoryUsage(int *textures)
{
	return mInternal->mVolumeBuffer.getMemoryUsage(textures);
}

ssc::GPUImageDataBufferPtr GPUImageBufferRepository::getGPUImageDataBuffer(vtkImageDataPtr volume)
{
	return mInternal->mVolumeBuffer.get(volume);
}

ssc::GPUImageLutBufferPtr GPUImageBufferRepository::getGPUImageLutBuffer(vtkUnsignedCharArrayPtr lut)
{
	return mInternal->mLutBuffer.get(lut);
}

}

#else

namespace ssc
{
void GPUImageBufferRepository::shutdown() {;}
void GPUImageBufferRepository::tearDown() {;}
}//namespace ssc

#endif //WIN32

namespace ssc
{

}
