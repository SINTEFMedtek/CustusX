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

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkLookupTable> vtkLookupTablePtr;
typedef vtkSmartPointer<class vtkScalarsToColors> vtkScalarsToColorsPtr;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPiecewiseFunction> vtkPiecewiseFunctionPtr;
typedef vtkSmartPointer<class vtkColorTransferFunction> vtkColorTransferFunctionPtr;
typedef vtkSmartPointer<class vtkVolumeProperty> vtkVolumePropertyPtr;
typedef vtkSmartPointer<class vtkUnsignedCharArray> vtkUnsignedCharArrayPtr;

class QColor;
class QDomDocument;
class QDomNode;

#include <map>
#include <boost/shared_ptr.hpp>
typedef std::map<int, int> IntIntMap;
typedef std::map<int, QColor> ColorMap;
typedef boost::shared_ptr<IntIntMap> OpacityMapPtr;
typedef boost::shared_ptr<ColorMap> ColorMapPtr;


namespace ssc
{

/**Handler for the transfer functions used in 3d image volumes.
 * Used by Image.
 */
class ImageTF3D : public QObject
{
	Q_OBJECT
public:
	ImageTF3D(vtkImageDataPtr base);
	void setVtkImageData(vtkImageDataPtr base);

	void setOpacityTF(vtkPiecewiseFunctionPtr tf);
	vtkPiecewiseFunctionPtr getOpacityTF();
	void setColorTF(vtkColorTransferFunctionPtr tf);
	vtkColorTransferFunctionPtr getColorTF();

	void setAlpha(double val);
	double getAlpha() const;	
	void setLLR(double val);
	double getLLR() const;
	void setWindow(double val);
	double getWindow() const;
	void setLevel(double val);
	double getLevel() const;
	void setLut(vtkLookupTablePtr lut);
	vtkLookupTablePtr getLut() const;
	double getScalarMax() const;
	double getScalarMin() const;///< \return Minimum intensity of underlying dataset
	
	OpacityMapPtr getOpacityMap();///< \return The values of the opacity transfer function
	ColorMapPtr getColorMap();///< \return The values of the color transfer function
	void addAlphaPoint( int alphaPosition , int alphaValue);///< Add point to the opacity transfer function
	void removeAlphaPoint(int alphaPosition);///< Remove point from the opacity transfer function
	void setAlphaValue(int alphaPosition, int alphaValue);///< Change value of an existing opacity transfer function point
	int getAlphaValue(int alphaPosition);///< \return Alpha value of a specified position in the opacity transfer function
	void addColorPoint( int colorPosition , QColor colorValue);///< Add point to the color transfer function
	void removeColorPoint(int colorPosition);///< Remove point from the color transfer function
	void setColorValue(int colorPosition, QColor colorValue);///< Change value of an existing color transfer function point

	QDomNode getXml(QDomDocument& doc);///< Create a XML node for this object. It's up to the caller to add this node to the XML tree. \return A XML data representation for this object. \param doc The root of the document tree.
	void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

signals:
	void transferFunctionsChanged();
	
private:
	void refreshColorTF();
	void refreshOpacityTF();

	//vtkPiecewiseFunctionPtr mGradientOpacityTF; // implement when needed.
	vtkPiecewiseFunctionPtr mOpacityTF;
	vtkColorTransferFunctionPtr mColorTF;
	vtkVolumePropertyPtr mVolumeProperty;
	
	vtkImageDataPtr mBase;
	double mLLR;
	double mWindow;
	double mLevel;
	double mAlpha;
	vtkLookupTablePtr mLut;
	
	OpacityMapPtr mOpacityMapPtr;
	ColorMapPtr mColorMapPtr;
};
	
typedef boost::shared_ptr<ImageTF3D> ImageTF3DPtr;

} // end namespace ssc

#endif /* SSCIMAGETRANSFERFUNCTIONS3D_H_ */
