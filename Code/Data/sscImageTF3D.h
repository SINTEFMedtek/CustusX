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

/*
 * sscImageTransferFunctions3D.h
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#ifndef SSCIMAGETRANSFERFUNCTIONS3D_H_
#define SSCIMAGETRANSFERFUNCTIONS3D_H_

#include <boost/shared_ptr.hpp>
#include <QObject>
#include "vtkForwardDeclarations.h"

class QColor;
class QDomDocument;
class QDomNode;

#include <map>
#include <boost/shared_ptr.hpp>
#include "sscImageTFData.h"

namespace ssc
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
 * \ingroup sscData
 */
class ImageTF3D: public ImageTFData
{
Q_OBJECT
public:
	ImageTF3D(vtkImageDataPtr base);

	void removeInitAlphaPoint();///< Remove the initial point set at 10%. Used when we need a "clean" transfer function.
	ImageTF3DPtr createCopy(vtkImageDataPtr newDataBase);

	vtkPiecewiseFunctionPtr getOpacityTF();
	vtkColorTransferFunctionPtr getColorTF();

	virtual void addXml(QDomNode dataNode); ///< adds xml information about the transferfunction and its variabels
	virtual void parseXml(QDomNode dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

signals:
	void transferFunctionsChanged();
private slots:
	void transferFunctionsChangedSlot();
private:
	ImageTF3D();
	virtual void LUTChanged();
	virtual void alphaLLRChanged();

	void buildOpacityMapFromLLRAlpha();

	void refreshColorTF();
	void refreshOpacityTF();

	//vtkPiecewiseFunctionPtr mGradientOpacityTF; // implement when needed.
	vtkPiecewiseFunctionPtr mOpacityTF;
	vtkColorTransferFunctionPtr mColorTF;
};

} // end namespace ssc

#endif /* SSCIMAGETRANSFERFUNCTIONS3D_H_ */
