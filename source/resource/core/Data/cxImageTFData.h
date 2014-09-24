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


/**\brief Data class for Transfer Function info, either 2D or 3D.
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
