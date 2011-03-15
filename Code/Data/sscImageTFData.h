/*
 * sscImageTFData.h
 *
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

//TODO: get an overview of the ranges of the alpha and color tfs, both index range and value range.

/**Data class for Transfer Function info, either 2D or 3D.
 * Used inside ImageTF3D and ImageLUT2D.
 */
class ImageTFData : public QObject
{
  Q_OBJECT
public:
  ImageTFData();
  virtual ~ImageTFData();
  ImageTFDataPtr createCopy();
  void initialize(double scalarMax);

  void   setAlpha(double val);
  double getAlpha() const;
  void   setLLR(double val);
  double getLLR() const;
  void   setWindow(double val);
  double getWindow() const;
  void   setLevel(double val);
  double getLevel() const;

  void fillColorTFFromMap(vtkColorTransferFunctionPtr tf);
  void fillOpacityTFFromMap(vtkPiecewiseFunctionPtr tf);

  OpacityMapPtr getOpacityMap();///< \return The values of the opacity transfer function
  ColorMapPtr getColorMap();///< \return The values of the color transfer function

  void addAlphaPoint( int alphaPosition , int alphaValue);///< Add point to the opacity transfer function
  void removeAlphaPoint(int alphaPosition);///< Remove point from the opacity transfer function
  void setAlphaValue(int alphaPosition, int alphaValue);///< Change value of an existing opacity transfer function point
  int  getAlphaValue(int alphaPosition);///< \return Alpha value of a specified position in the opacity transfer function
  void addColorPoint( int colorPosition , QColor colorValue);///< Add point to the color transfer function
  void removeColorPoint(int colorPosition);///< Remove point from the color transfer function
  void setColorValue(int colorPosition, QColor colorValue);///< Change value of an existing color transfer function point

  void addXml(QDomNode dataNode); ///< adds xml information about the transferfunction and its variabels
  void parseXml(QDomNode dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

signals:
  void changed();

private:
  // these values can be used instead of setting the opacity TF explicitly
  double mLLR;
  double mWindow;
  double mLevel;
  double mAlpha;
 // vtkLookupTablePtr mLut;

  OpacityMapPtr mOpacityMapPtr;
  ColorMapPtr mColorMapPtr;
};

}

#endif /* SSCIMAGETFDATA_H_ */
