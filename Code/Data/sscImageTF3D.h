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
//typedef std::map<int, int> IntIntMap;
//typedef std::map<int, QColor> ColorMap;
//typedef boost::shared_ptr<IntIntMap> OpacityMapPtr;
//typedef boost::shared_ptr<ColorMap> ColorMapPtr;
#include "sscImageTFData.h"

namespace ssc
{

typedef boost::shared_ptr<class ImageTF3D> ImageTF3DPtr;

/**Handler for the transfer functions used in 3d image volumes.
 * Used by Image.
 */
class ImageTF3D : public QObject
{
	Q_OBJECT
public:
	ImageTF3D(vtkImageDataPtr base);
	void setVtkImageData(vtkImageDataPtr base);

	ImageTF3DPtr createCopy();

	vtkPiecewiseFunctionPtr getOpacityTF();
	vtkColorTransferFunctionPtr getColorTF();
  ImageTFDataPtr getData();

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

	void addXml(QDomNode dataNode); ///< adds xml information about the transferfunction and its variabels
	void parseXml(QDomNode dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

signals:
	void transferFunctionsChanged();
private slots:
  void transferFunctionsChangedSlot();
private:
	void refreshColorTF();
	void refreshOpacityTF();

	//vtkPiecewiseFunctionPtr mGradientOpacityTF; // implement when needed.
	vtkPiecewiseFunctionPtr mOpacityTF;
	vtkColorTransferFunctionPtr mColorTF;
	
	vtkImageDataPtr mBase;
	vtkLookupTablePtr mLut;
	ImageTFDataPtr mData;
};
	

} // end namespace ssc

#endif /* SSCIMAGETRANSFERFUNCTIONS3D_H_ */
