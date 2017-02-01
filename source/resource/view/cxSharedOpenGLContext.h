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

#ifndef CXSHAREDOPENGLCONTEXT_H
#define CXSHAREDOPENGLCONTEXT_H

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glew.h>

#include <boost/shared_ptr.hpp>
#include "vtkForwardDeclarations.h"

#include "cxResourceVisualizationExport.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

namespace cx
{

typedef boost::shared_ptr<class SharedOpenGLContext> SharedOpenGLContextPtr;

/**
 * @brief Shared OpenGL context
 *
 * CX use a shared OpenGL context for all render windows.
 * This is a hack added to our branch of VTK.
 * See vtkXOpenGLRenderWindow (Linux), vtkCocoaRenderWindow (Mac), and "vtkSomething" (Windows)
 *
 * There exist only one shared OpenGL context, and this is set to be the id of the first context created by vtkRenderWindow.
 * All vtkRenderWindows created gets this shared context.
 * This means that the first vtkRenderWindow MUST NOT be deleted, as it contains THE OpenGL context.
 */
class cxResourceVisualization_EXPORT SharedOpenGLContext
{
public:
	static bool isValid(vtkOpenGLRenderWindowPtr opengl_renderwindow, bool print=false);

	SharedOpenGLContext(vtkOpenGLRenderWindowPtr sharedContext);
	~SharedOpenGLContext();

	bool makeCurrent() const;
	int getNumberOfTexturesInUse() const;

	//Image textures are per image
	bool uploadImage(ImagePtr image);
	bool hasUploadedImage(QString image_uid) const;
	vtkTextureObjectPtr get3DTextureForImage(QString image_uid) const;
	bool delete3DTextureForImage(QString image_uid);

	//LUT textures are per image
	bool uploadLUT(QString imageUid, vtkUnsignedCharArrayPtr lutTable);
	bool hasUploadedLUT(QString image_uid) const;
	vtkTextureObjectPtr get1DTextureForLUT(QString image_uid) const;
	bool delete1DTextureForLUT(QString image_uid);

	//Texture coordinates are per view
	bool upload3DTextureCoordinates(QString uid, vtkFloatArrayPtr texture_coordinates);
	bool hasUploadedTextureCoordinates(QString uid) const;
	vtkOpenGLBufferObjectPtr getTextureCoordinates(QString uid) const;
	vtkImageDataPtr downloadImageFromTextureBuffer(QString image_uid);//For testing


private:
	bool create1DTextureObject(vtkTextureObjectPtr texture_object, unsigned int width, int dataType, int numComps, void *data, vtkOpenGLRenderWindowPtr opengl_renderwindow) const;
	bool create3DTextureObject(vtkTextureObjectPtr texture_object, unsigned int width, unsigned int height, unsigned int depth, int dataType, int numComps, void *data, vtkOpenGLRenderWindowPtr opengl_renderwindow) const;
	vtkOpenGLBufferObjectPtr allocateAndUploadArrayBuffer(QString uid, int my_numberOfTextureCoordinates, int numberOfComponentsPerTexture, const float *texture_data) const;

	/**
	 * QString - uid of the texture
	 * vtkTextureObjectPtr - pointer to the texture
	 * unsigned long - time of modification of uploaded data
	 */
	std::map<QString, std::pair<vtkTextureObjectPtr, unsigned long> > m1DTextureObjects;
	std::map<QString, std::pair<vtkTextureObjectPtr, unsigned long> > m3DTextureObjects;

	std::map<QString, vtkOpenGLBufferObjectPtr > mTextureCoordinateBuffers;

	vtkOpenGLRenderWindowPtr mContext;

	SharedOpenGLContext(); //not implemented
};

}//cx

#endif // CXSHAREDOPENGLCONTEXT_H
