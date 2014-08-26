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


#ifndef CXIMAGETF3D_H_
#define CXIMAGETF3D_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include "vtkForwardDeclarations.h"

class QColor;
class QDomDocument;
class QDomNode;

#include <map>
#include <boost/shared_ptr.hpp>
#include "cxImageTFData.h"

namespace cx
{

typedef boost::shared_ptr<class ImageTF3D> ImageTF3DPtr;


/**\brief Handler for the transfer functions used in 3d image volumes.
 *
 * Used by Image.
 *
 * Set the basic lut using either setLut() or setColorPoint(), then modify it with window and level.
 * Set the alpha channel using setAlphaPoint(), or override it by creating a opacity step function
 * with LLR and Alpha.
 *
 * The volume rendering classes can use the data by getting OpacityTF and ColorTF.
 *
 * \ingroup cx_resource_core_data
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */
class ImageTF3D: public ImageTFData
{
Q_OBJECT
public:
	ImageTF3D();

//	void setInitialTFFromImage(vtkImageDataPtr base);
	ImageTF3DPtr createCopy();

	vtkPiecewiseFunctionPtr getOpacityTF();
	vtkColorTransferFunctionPtr getColorTF();

protected:
	virtual void internalsHaveChanged();
private:
	void buildOpacityMapFromLLRAlpha();
	void refreshColorTF();
	void refreshOpacityTF();

	//vtkPiecewiseFunctionPtr mGradientOpacityTF; // implement when needed.
	vtkPiecewiseFunctionPtr mOpacityTF;
	vtkColorTransferFunctionPtr mColorTF;
};

} // end namespace cx

#endif /* CXIMAGETF3D_H_ */
