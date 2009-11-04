#include "sscGPUImageBuffer.h"

#include <vector>
#include <vtkImageData.h>
#include <vtkgl.h>
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnsignedShortArray.h>
#include <stdint.h>
#include <GL/glu.h>

#define report_gl_error() fgl_really_report_gl_errors(__FILE__, __LINE__)

namespace{
	void fgl_really_report_gl_errors( const char *file, int line )
	{
		GLenum error;
		int i = 0;

		while ( ( error = glGetError () ) != GL_NO_ERROR  && i < 20 )
		{
			printf( "Oops, GL error caught: %s %s:%d\n", gluErrorString( error ), file, line );
			++i;
		}
		if ( i != 0 )
		{
			abort();
		}
	}
}

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

	GPUImageDataBufferImpl()
	{
		//std::cout << "create GPUImageDataBufferImpl()" << std::endl;
		mAllocated = false;
	}
	virtual ~GPUImageDataBufferImpl()
	{
		//std::cout << "delete GPUImageDataBufferImpl()" << std::endl;
		release();
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

		GLenum size,internalType;
		uint32_t dimx = mTexture ->GetDimensions( )[0];
		uint32_t dimy = mTexture ->GetDimensions( )[1];
		uint32_t dimz = mTexture ->GetDimensions( )[2];

		glEnable( vtkgl::TEXTURE_3D );
		glGenTextures(1, &textureId);

		//vtkgl::ActiveTexture(getGLTextureForVolume(textureUnitIndex)); //TODO is this OK?

		glBindTexture(vtkgl::TEXTURE_3D, textureId);
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
		}
			break; //16UI_EXT; break;
		default:
			std::cout << "Bit size not supported!" << std::endl;
			break;
		}
		
		if (mTexture->GetNumberOfScalarComponents()==1)
		{
			void* data = mTexture->GetPointData()->GetScalars()->GetVoidPointer(0);
			vtkgl::TexImage3D(vtkgl::TEXTURE_3D, 0, internalType, dimx, dimy, dimz, 0, GL_LUMINANCE, size, data);
		}
		else if (mTexture->GetNumberOfScalarComponents()==3)
		{
			internalType = GL_RGB;
			void* data = mTexture->GetPointData()->GetScalars()->GetVoidPointer(0);
			vtkgl::TexImage3D(vtkgl::TEXTURE_3D, 0, internalType, dimx, dimy, dimz, 0, GL_RGB, size, data);
		}
		else
		{
			std::cout << "unsupported number of image components" << std::endl;
		}
//
		glBindTexture(GL_TEXTURE_3D, 0);
		glDisable(GL_TEXTURE_3D);

		report_gl_error();
		std::cout << "this=[" << this << "] tex=[" << mTexture << "] allocated buffer" << std::endl;

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
			std::cout << "error: called bind() on unallocated volume buffer" << std::endl;
			return;
		}
		vtkgl::ActiveTexture(getGLTextureForVolume(textureUnitIndex));
		glBindTexture(GL_TEXTURE_3D, textureId);
		report_gl_error();
	}

	virtual void release()
	{
		//std::cout<< "GPUImageDataBuffer::release"<<std::endl;
		if (glIsTexture(textureId) )
		{
			//std::cout << "glHasTexture " << std::endl;
			glBindTexture(GL_TEXTURE_3D,0);
			glDisable(GL_TEXTURE_3D);
			glDisable(GL_TEXTURE_2D);
			//glDeleteTextures(1, &textureId);
		}
		
		std::cout << "this=[" << this << "] tex=[" << mTexture << "] release buffer" << std::endl;
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

	GPUImageLutBufferImpl()
	{
		//std::cout << "create GPUImageLutBufferImpl()" << std::endl;
		mAllocated = false;
		mLutSize = 0;
	}
	virtual ~GPUImageLutBufferImpl()
	{
		//std::cout << "delete GPUImageLutBufferImpl()" << std::endl;
		release();
	}

	//intput lookuptable is raw imported table
	virtual void SetColorMap(vtkUnsignedCharArrayPtr table)
	{
		mLutSize = table->GetNumberOfTuples();
		//std::cout << "setting color map: " << mLutSize << std::endl;
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
//
		glBindTexture(vtkgl::TEXTURE_BUFFER_EXT, textureId);
		vtkgl::TexBufferEXT(vtkgl::TEXTURE_BUFFER_EXT, vtkgl::RGBA32F_ARB, lutBuffer);
		report_gl_error();

		glBindTexture(vtkgl::TEXTURE_BUFFER_EXT, 0);

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
		//std::cout<< "GPUImageDataBuffer::release"<<std::endl;
		glBindTexture(vtkgl::TEXTURE_BUFFER_EXT,0);		
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
		
		std::cout << "current gpu buffer count: " << mData.size() << "+" << mRemovedData.size() << std::endl;
		
		return retval;
	}
	
private:
	typedef std::map<DATA_PTR, BufferWeakPtr> BufferMap;
	BufferMap mRemovedData; // those buffers that are removed but still might live outside of the repo.
	//typedef std::map<DATA_PTR, BufferWeakPtr> RemovedDataIter;
	std::list<BufferStore> mData; // newest elems in front
	//std::map<DATA_PTR, BufferPtr> mData; // repository of buffered buffers.
	unsigned mMaxBuffers;
	
};

class GPUImageBufferRepositoryInternal
{
public:
	BufferQueue<vtkImageDataPtr, ssc::GPUImageDataBuffer> mVolumeBuffer;
	BufferQueue<vtkUnsignedCharArrayPtr, ssc::GPUImageLutBuffer> mLutBuffer;
};

GPUImageBufferRepository::GPUImageBufferRepository()
{
	mInternal = new GPUImageBufferRepositoryInternal();
//	mMaxVolumes = 7;
//	mMaxLuts = 7;
}

GPUImageBufferRepository* GPUImageBufferRepository::getInstance()
{
	if (!mInstance)
	{
		mInstance = new GPUImageBufferRepository();
	}
	return mInstance;
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
