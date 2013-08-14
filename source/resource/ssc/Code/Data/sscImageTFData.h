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
 *  Created on: Mar 15, 2011
 *      Author: christiana
 */

#ifndef SSCIMAGETFDATA_H_
#define SSCIMAGETFDATA_H_

#include <map>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QColor>
#include "vtkForwardDeclarations.h"
class QDomNode;

namespace ssc
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
 * \ingroup sscData
 */
class ImageTFData: public QObject
{
Q_OBJECT
public:
	ImageTFData(vtkImageDataPtr base);
	void setVtkImageData(vtkImageDataPtr base);
	vtkImageDataPtr getVtkImageData();
	virtual ~ImageTFData();

	void setAlpha(double val); ///< range [0..1]
	double getAlpha() const;
	void setLLR(double val); ///< range [scalarMin..scalarMax]
	double getLLR() const;
	void setWindow(double val); ///< range [1..scalarMax-scalarMin]
	double getWindow() const;
	void setLevel(double val); ///< range [scalarMin..scalarMax]
	double getLevel() const;

	void setLut(vtkLookupTablePtr lut);
	vtkLookupTablePtr getLut() const;

	double getScalarMax() const;
	double getScalarMin() const;///< \return Minimum intensity of underlying dataset
	int getMaxAlphaValue() const; // dont use

	OpacityMapPtr getOpacityMap();///< \return The values of the opacity transfer function. Key range [scalarMin..scalarMax], Value range [0..255].
	ColorMapPtr getColorMap();///< \return The values of the color transfer function. Key range [scalarMin..scalarMax].

	void addAlphaPoint(int alphaPosition, int alphaValue);///< Add point to the opacity transfer function
	void removeAlphaPoint(int alphaPosition);///< Remove point from the opacity transfer function
	void moveAlphaPoint(int oldpos, int newpos, int alphaValue);
	void addColorPoint(int colorPosition, QColor colorValue);///< Add point to the color transfer function
	void removeColorPoint(int colorPosition);///< Remove point from the color transfer function
	void moveColorPoint(int oldpos, int newpos, QColor colorValue);

	virtual void addXml(QDomNode dataNode); ///< adds xml information about the transferfunction and its variabels
	virtual void parseXml(QDomNode dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

	void unsignedCT(bool onLoad);
	void shift(int val); ///< shift the transfter function index values by the input amount. Used for signed/unsigned conversion.

	void fixTransferFunctions(); ///< Modify/repair transfer function so it matches current image
	void fillColorTFFromMap(vtkColorTransferFunctionPtr tf);

signals:
	void changed();

protected:
	virtual void colorMapChanged();
	virtual void alphaLLRChanged()
	{
	} // implemented by subclass
	virtual void LUTChanged()
	{
	} // implemented by subclass

	void buildLUTFromColorMap();
	void deepCopy(ImageTFData* source);

	void fillOpacityTFFromMap(vtkPiecewiseFunctionPtr tf);
	void fillLUTFromLut(vtkLookupTablePtr output, vtkLookupTablePtr input);

	vtkImageDataPtr mBase;
	OpacityMapPtr mOpacityMapPtr;
	ColorMapPtr mColorMapPtr;
	vtkLookupTablePtr mLut;

private:
	// these values can be used instead of setting the opacity TF explicitly
	double mLLR;
	double mWindow;
	double mLevel;
	double mAlpha;
	double loadAttribute(QDomNode dataNode, QString name, double defVal);
	double mapThroughLUT(double x, int lutSize);
};

}

#endif /* SSCIMAGETFDATA_H_ */
