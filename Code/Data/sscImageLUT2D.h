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

/**
 * Handling of color and opacity for 2D slices.
 * Set the basic lut using either setLut() or setColorMap(), then modify it with window and level.
 * The alpha channel in this lut is ignored.
 * The opacity is controlled with LLR and Alpha values, which creates a step function for the opacity.
 * The AlphaMap from the superclass is _not_ used, it is overwritten by setting llr/alpha.
 *
 * The slicer classes will use the data either by getting the LUT+parameters(win/lvl(llr/alpha) and,
 * or by getting OutputLookupTable() which is a merge of the LUT and parameters.
 */
class ImageLUT2D : public ImageTFData
{
	Q_OBJECT
public:
	ImageLUT2D(vtkImageDataPtr base);
	ImageLUT2DPtr createCopy();

	void setBaseLookupTable(vtkLookupTablePtr lut); //</ backwards compatibility: prefer setLut()
	vtkLookupTablePtr getOutputLookupTable();
	vtkLookupTablePtr getBaseLookupTable(); ///< backwards compatibility: prefer getLut()

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the image and its variabels \param parentNode Parent node in the XML tree \return The created subnode
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
