/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXIMAGETF3D_H_
#define CXIMAGETF3D_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

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


/** \brief Handler for the transfer functions used in 3d image volumes.
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
class cxResource_EXPORT ImageTF3D: public ImageTFData
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
