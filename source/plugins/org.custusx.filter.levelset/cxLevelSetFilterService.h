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

#ifndef CXLEVELSETFILTER_H_
#define CXLEVELSETFILTER_H_

#include "cxFilterImpl.h"
#include "cxVector3D.h"
#include "org_custusx_filter_levelset_Export.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
namespace cx
{

class org_custusx_filter_levelset_EXPORT LevelSetFilter: public FilterImpl
{
Q_OBJECT

public:
	virtual ~LevelSetFilter()
	{
	}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
	virtual void setActive(bool on);

	bool preProcess();
	virtual bool execute();
	virtual bool postProcess();

	// extensions:
	DoubleDataAdapterXmlPtr getThresholdOption(QDomElement root);
	DoubleDataAdapterXmlPtr getEpsilonOption(QDomElement root);
	DoubleDataAdapterXmlPtr getAlphaOption(QDomElement root);
	DoubleDataAdapterXmlPtr getRadiusOption(QDomElement root);

	static Vector3D getSeedPointFromTool(DataPtr image);
	static bool isSeedPointInsideImage(Vector3D, DataPtr);
	QDomElement getmOptions();

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();
private:
	vtkImageDataPtr convertToVtkImageData(char * data, int size_x, int size_y,
			int size_z, ImagePtr input); ///< converts a char array to a vtkImageDataPtr
	vtkImageDataPtr importRawImageData(void * data, int size_x, int size_y,
			int size_z, ImagePtr input, int type); ///< converts a any array to a vtkImageDataPtr
	std::string filename;
	Vector3D seedPoint;
	ImagePtr image;
	vtkImageDataPtr rawSegmentation;

};
// end LevelSetFilter class

typedef boost::shared_ptr<class LevelSetFilter> LevelSetFilterPtr;

} // end namespace

#endif /* CXLEVELSETFILTER_H_ */
