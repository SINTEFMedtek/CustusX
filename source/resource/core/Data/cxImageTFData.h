/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


/*
 *  Created on: Mar 15, 2011
 *      Author: christiana
 */

#ifndef CXIMAGETFDATA_H_
#define CXIMAGETFDATA_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include <map>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QColor>
#include "vtkForwardDeclarations.h"
class QDomNode;

namespace cx
{
typedef std::map<int, int> IntIntMap;
typedef std::map<int, QColor> ColorMap;
typedef boost::shared_ptr<IntIntMap> OpacityMapPtr;
typedef boost::shared_ptr<ColorMap> ColorMapPtr;

typedef boost::shared_ptr<class ImageTFData> ImageTFDataPtr;


/** \brief Data class for Transfer Function info, either 2D or 3D.
 *
 * Used as base for ImageTF3D and ImageLUT2D.
 *
 * Alpha transfer function: Set either LLR/alpha or OpacityMap, this updates
 * the data as shown.
 *
 *
 * \verbatim

      set            set
       |              |
    LLR/alpha --> OpacityMap --> alphaTF

  \endverbatim
 *
 *
 * Color transfer function: Set either ColorMap or LUT, plus the Win/Level,
 * this updates the data as shown.
 *
 * \verbatim

      set        set
       |          |
    ColorMap --> LUT -------> colorTF
                          |
               Win/Level--|
                  |
                 set

  \endverbatim
 *
 *
 * \ingroup cx_resource_core_data
 */
class cxResource_EXPORT ImageTFData: public QObject
{
Q_OBJECT
public:
	ImageTFData();
	virtual ~ImageTFData();

	void setAlpha(double val); ///< range [0..1]
	double getAlpha() const;
	void setLLR(double val); ///< range [scalarMin..scalarMax]
	double getLLR() const;
	void setWindow(double val); ///< range [1..scalarMax-scalarMin]
	double getWindow() const;
	void setLevel(double val); ///< range [scalarMin..scalarMax]
	double getLevel() const;

	IntIntMap getOpacityMap();///< \return The values of the opacity transfer function. Key range [scalarMin..scalarMax], Value range [0..255].
	ColorMap getColorMap();///< \return The values of the color transfer function. Key range [scalarMin..scalarMax].

	void resetAlpha(IntIntMap val);
	void addAlphaPoint(int alphaPosition, int alphaValue);///< Add point to the opacity transfer function
	void removeAlphaPoint(int alphaPosition);///< Remove point from the opacity transfer function
	void moveAlphaPoint(int oldpos, int newpos, int alphaValue);

	void resetColor(ColorMap val);
	void addColorPoint(int colorPosition, QColor colorValue);///< Add point to the color transfer function
	void removeColorPoint(int colorPosition);///< Remove point from the color transfer function
	void moveColorPoint(int oldpos, int newpos, QColor colorValue);

	virtual void addXml(QDomNode dataNode); ///< adds xml information about the transferfunction and its variabels
	virtual void parseXml(QDomNode dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

	void unsignedCT(bool onLoad);
	void shift(int val); ///< shift the transfter function index values by the input amount. Used for signed/unsigned conversion.

	vtkColorTransferFunctionPtr generateColorTF() const;
	vtkPiecewiseFunctionPtr generateOpacityTF() const;

signals:
	void transferFunctionsChanged();

protected:
	void deepCopy(ImageTFData* source);
	virtual void internalsHaveChanged() {}

	void fillOpacityTFFromMap(vtkPiecewiseFunctionPtr tf) const;
	void fillColorTFFromMap(vtkColorTransferFunctionPtr tf) const;
	void shiftColor(int shift, double center, double scale);
	void shiftOpacity(int shift);

	IntIntMap mOpacityMap;
	ColorMap mColorMap;
};

}

#endif /* CXIMAGETFDATA_H_ */
