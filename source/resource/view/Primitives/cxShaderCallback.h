/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXSINGLEVOLUMEPAINTERHELPER_H
#define CXSINGLEVOLUMEPAINTERHELPER_H

#include "cxResourceVisualizationExport.h"
#include "cxForwardDeclarations.h"
#include <vtkCommand.h>

#include "cxLogger.h"


class vtkOpenGLHelper;
class vtkShaderProgram;
class vtkOpenGLVertexArrayObject;

typedef vtkSmartPointer<class vtkTextureObject> vtkTextureObjectPtr;
typedef vtkSmartPointer<class vtkOpenGLBufferObject> vtkOpenGLBufferObjectPtr;

namespace cx
{
typedef vtkSmartPointer<class ShaderCallback> ShaderCallbackPtr;

/**
 * @brief The ShaderCallback class is used to update information sent to our
 * custom OpenGL shaders.
 */
class cxResourceVisualization_EXPORT ShaderCallback : public vtkCommand
{

public:
	/**
	 * Conventient structure that groups relevant information for our custom OpenGL shaders on a per view basis.
	 */
	struct ShaderItem
	{
		QString mTextureUid;
		vtkTextureObjectPtr mTexture;

		QString mTextureCoordinatesUid;
		vtkOpenGLBufferObjectPtr mTextureCoordinates;

		QString mLUTUid;
		vtkTextureObjectPtr mLUT;

		//int mLutSize; //basically: used to check if lut should be applied, also a check if it is RED or RGB(A)
		float mWindow;
		float mLevel;
		float mLLR; //low level reject
		float mAlpha;
	};
	typedef boost::shared_ptr<ShaderItem> ShaderItemPtr;

	static ShaderCallback *New();
	ShaderCallback();
	virtual ~ShaderCallback();

	virtual void Execute(vtkObject *, unsigned long eventId, void*cbo);

	ShaderItemPtr getShaderItem(QString image_uid) const;
	int getNumberOfUploadedTextures() const;

	void add(ShaderItemPtr item);
	void clearShaderItems();

private:
	static void addArrayToAttributeArray(vtkShaderProgram *program, vtkOpenGLBufferObjectPtr buffer, std::string name, int vector_index);
	static void addUniformiArray(vtkShaderProgram *program, std::string name, int value);
	static void addUniformfArray(vtkShaderProgram *program, std::string name, float value);
	static void bindFSOutputVariable(vtkShaderProgram *program);

	std::string getVectorNameFromName(std::string name, int index_of_vector) const;
	void printDebugInfo(vtkOpenGLHelper *OpenGLHelper);

	std::vector<ShaderItemPtr> mShaderItems;
};

}//cx

#endif // CXSINGLEVOLUMEPAINTERHELPER_H
