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

/**Handler for the lookup table used in 2d image slices.
 * This lookup-handler has default lut grayscale
 * Or use stored lut from file.
 * This class will also store differenst windowlevel values and color.
 */
class ImageLUT2D : public QObject
{
	Q_OBJECT
public:
	ImageLUT2D(vtkImageDataPtr base);
	void setVtkImageData(vtkImageDataPtr base);
	ImageLUT2DPtr createCopy();
  ImageTFDataPtr getData();

	void setBaseLookupTable(vtkLookupTablePtr lut);
	vtkLookupTablePtr getOutputLookupTable();
	vtkLookupTablePtr getBaseLookupTable();
	void setLLR(double val);
	void setWindow(double val);
	void setLevel(double val);
	void setAlpha(double val);
	double getWindow() const;
	double getLLR() const;
	double getLevel() const;
	double getAlpha() const;

	double getScalarMax() const;

  void addXml(QDomNode& dataNode); ///< adds xml information about the image and its variabels \param parentNode Parent node in the XML tree \return The created subnode
  void parseXml(QDomNode dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

signals:
	void transferFunctionsChanged();

private slots:
  void transferFunctionsChangedSlot();

private:
//	double loadAttribute(QDomNode dataNode, QString name, double defVal);
	void refreshOutput();
//	void changeOpacity(double index, double opacity);
//	void testMap(double val);

	vtkLookupTablePtr mOutputLUT; ///< the sum of all internal values
	vtkImageDataPtr mBase; ///< image data
//	double mLLR; ///< used to modify the output
//	double mWindow; ///< used to modify the output
//	double mLevel; ///< used to modify the output
//	double mAlpha; ///< not used by output, use BlenderRep to solve this.
	vtkLookupTablePtr mBaseLUT;	///< basis for generating the output lut
  ImageTFDataPtr mData;
};



}

#endif /* SSCIMAGELOOKUPTABLE2D_H_ */
