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

/**Handler for the transfer functions used in 3d image volumes.
 * Used by Image.
 *
 * Set the basic lut using either setLut() or setColorPoint(), then modify it with window and level.
 * Set the alpha channel using setAlphaPoint(), or override it by creating a opacity step function
 * with LLR and Alpha.
 *
 * The volume rendering classes can use the data by getting OpacityTF and ColorTF.
 */
class ImageTF3D : public ImageTFData
{
	Q_OBJECT
public:
	ImageTF3D(vtkImageDataPtr base);

	ImageTF3DPtr createCopy();

	vtkPiecewiseFunctionPtr getOpacityTF();
	vtkColorTransferFunctionPtr getColorTF();

	virtual void addXml(QDomNode dataNode); ///< adds xml information about the transferfunction and its variabels
	virtual void parseXml(QDomNode dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

signals:
	void transferFunctionsChanged();
private slots:
  void transferFunctionsChangedSlot();
private:
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
