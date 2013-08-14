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
 * sscImageLookupTable2D.h
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#ifndef SSCIMAGELOOKUPTABLE2D_H_
#define SSCIMAGELOOKUPTABLE2D_H_

class QDomElement;
class QDomNode;

#include <boost/shared_ptr.hpp>
#include <QColor>
#include <QObject>
#include "vtkForwardDeclarations.h"
#include "sscImageTFData.h"

namespace ssc
{
typedef boost::shared_ptr<class ImageLUT2D> ImageLUT2DPtr;


/**\brief Handling of color and opacity for 2D slices.
 *
 * Set the basic lut using either setLut() or setColorMap(), then modify it with window and level.
 * The alpha channel in this lut is ignored.
 * The opacity is controlled with LLR and Alpha values, which creates a step function for the opacity.
 * The AlphaMap from the superclass is _not_ used, it is overwritten by setting llr/alpha.
 *
 * The slicer classes will use the data either by getting the LUT+parameters(win/lvl(llr/alpha) and,
 * or by getting OutputLookupTable() which is a merge of the LUT and parameters.
 *
 * \ingroup sscData
 */
class ImageLUT2D: public ImageTFData
{
Q_OBJECT
public:
	ImageLUT2D(vtkImageDataPtr base);
	ImageLUT2DPtr createCopy(vtkImageDataPtr newBase);
	void setFullRangeWinLevel(); ///< Set winlevel spanning the entire range

	void setBaseLookupTable(vtkLookupTablePtr lut); //</ backwards compatibility: prefer setLut()
	vtkLookupTablePtr getOutputLookupTable();
	vtkLookupTablePtr getBaseLookupTable(); ///< backwards compatibility: prefer getLut()

	virtual void addXml(QDomNode dataNode); ///< adds xml information about the image and its variabels \param dataNode data node in the XML tree \return The created subnode
	virtual void parseXml(QDomNode dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

signals:
	void transferFunctionsChanged();

private slots:
	void transferFunctionsChangedSlot();

private:
	virtual void alphaLLRChanged();
	void buildOpacityMapFromLLRAlpha();
	void refreshOutput();
	void LUTChanged();

	vtkLookupTablePtr mOutputLUT; ///< the sum of all internal values
};

}

#endif /* SSCIMAGELOOKUPTABLE2D_H_ */
