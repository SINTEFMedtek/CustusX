/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
