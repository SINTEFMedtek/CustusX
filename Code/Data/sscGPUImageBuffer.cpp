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
class GPUImageBufferImpl : public GPUImageBuffer
{
public:
	GLuint lutBuffer;
	GLuint textures[2];

	vtkImageDataPtr mTexture;
	int mLutDataSize;
	int mLutSize;
	std::vector<float> mLut;
	bool mAllocated;
	//int mTextureUnit; ///< index of GL_TEXTURE<2X> and GL_TEXTURE<2X+1> used for volume and lut

	GPUImageBufferImpl()
	{
		std::cout << "create GPUImageBufferImpl()" << std::endl;
		mAllocated = false;
		mLutSize = 0.0;
	//	mTextureUnit = unit;
	}
	virtual ~GPUImageBufferImpl()
	{
		std::cout << "delete GPUImageBufferImpl()" << std::endl;
		release();
	}

	//intput lookuptable is raw imported table
	virtual void SetColorMap(vtkUnsignedCharArrayPtr table)
	{
		mLutSize = table->GetNumberOfTuples();
		mLutDataSize = mLutSize * table->GetNumberOfComponents();
		mLut.resize(mLutDataSize);

		//Logger::log("vm.log"," Lut size "+ string_cast(mLutSize)+ ", Lut data size "+ string_cast(mLutDataSize) );

		unsigned char* ptr = table->GetPointer(0);

		for (int i = 0; i < mLutDataSize; ++i)
		{
			mLut[i] = ((float) *ptr) / 255.0;
			++ptr;
		}
	}

	virtual void SetImage (vtkImageDataPtr texture)
	{
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
		if (!mTexture || mLut.empty())
		{
			std::cout << "error: bad buffer initialization" << std::endl;
			return;
		}

		vtkgl::ActiveTexture(GL_TEXTURE7);

		GLenum size,internalType;
		uint32_t dimx = mTexture ->GetDimensions( )[0];
		uint32_t dimy = mTexture ->GetDimensions( )[1];
		uint32_t dimz = mTexture ->GetDimensions( )[2];

		glEnable( vtkgl::TEXTURE_3D );
		glGenTextures(2, textures);

		//vtkgl::ActiveTexture(getGLTextureForVolume(textureUnitIndex)); //TODO is this OK?

		glBindTexture(vtkgl::TEXTURE_3D, textures[0]);
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
		void* data = mTexture->GetPointData()->GetScalars()->GetVoidPointer(0);
		glTexImage3D(vtkgl::TEXTURE_3D, 0, internalType, dimx, dimy, dimz, 0, GL_LUMINANCE, size, data);
//
		glBindTexture(GL_TEXTURE_3D, 0);
		glDisable(GL_TEXTURE_3D);

		/** upload color buffer **/
		vtkgl::ActiveTexture(GL_TEXTURE8);
		//vtkgl::ActiveTexture(getGLTextureForLut(textureUnitIndex)); //TODO is this OK?

		vtkgl::GenBuffersARB(1, &lutBuffer);
		vtkgl::BindBuffer(vtkgl::TEXTURE_BUFFER_EXT, lutBuffer);
		vtkgl::BufferData(vtkgl::TEXTURE_BUFFER_EXT, mLutDataSize * sizeof(float), &(*mLut.begin()), vtkgl::STATIC_DRAW);
//
		glBindTexture(vtkgl::TEXTURE_BUFFER_EXT, textures[1]);
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
			std::cout << "error: called bind() on unallocated buffer" << std::endl;
			return;
		}

		//glEnable( vtkgl::TEXTURE_3D );

		vtkgl::ActiveTexture(getGLTextureForVolume(textureUnitIndex));
		glBindTexture(GL_TEXTURE_3D, textures[0]);

		vtkgl::ActiveTexture(getGLTextureForLut(textureUnitIndex));
		glBindTexture(vtkgl::TEXTURE_BUFFER_EXT, textures[1]);
		report_gl_error();

		//glDisable( vtkgl::TEXTURE_3D );
	}

	void release()
	{
		glDeleteTextures(2, textures);
		//glDeleteBuffersARB(1, &lutBuffer); //TODO find a working deleter
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

GPUImageBufferPtr createGPUImageBuffer(vtkImageDataPtr volume, vtkUnsignedCharArrayPtr lut)
{
	std::auto_ptr<GPUImageBufferImpl> retval(new GPUImageBufferImpl);
	retval->SetImage(volume);
	retval->SetColorMap(lut);
	return GPUImageBufferPtr(retval.release());
}


}
