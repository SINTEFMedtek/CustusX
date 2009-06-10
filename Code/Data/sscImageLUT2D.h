/*
 * sscImageLookupTable2D.h
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#ifndef SSCIMAGELOOKUPTABLE2D_H_
#define SSCIMAGELOOKUPTABLE2D_H_

#include <QColor>
#include <vtkUnsignedCharArray.h> 
#include "vtkSmartPointer.h" 
typedef vtkSmartPointer<class vtkLookupTable> vtkLookupTablePtr;
typedef vtkSmartPointer<class vtkWindowLevelLookupTable> vtkWindowLevelLookupTablePtr;
typedef vtkSmartPointer<class vtkScalarsToColors> vtkScalarsToColorsPtr;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPiecewiseFunction> vtkPiecewiseFunctionPtr;
typedef vtkSmartPointer<class vtkImageMapToColors> vtkImageMapToColorsPtr;
namespace ssc
{

/**Handler for the lookup table used in 2d image slices.
 * This lookup-handler has default lut grayscale
 * Or usese stored lut from file.
 * This class will also store differenst windowlevel values and color.
 */
class ImageLUT2D
{
public:
	ImageLUT2D(vtkImageDataPtr base);

	void setLookupTable(vtkLookupTablePtr lut);
	vtkLookupTablePtr getLookupTable(); //try to be use ony one class rep for lut--
	void setLLR(double val);
	void setWindow(double window);
	void setLevel(double level);
	double getWindow() const;
	double getLLR() const;
	double getLevel() const;
	double getScalarMax() const;
	void setAlphaRange(double alpha) ;
	void changeOpacityForAll(double opacity);
	void changeOpacity(double index, double opacity);
	void setTable(vtkUnsignedCharArray *  table);
	//void setImportedLut(vtkLookupTablePtr lut); //temporary method....
	
	vtkImageMapToColorsPtr getColorMap();

private:
	void printToFile();
	//vtkScalarsToColorsPtr mScalarToColor;
	//vtkLookupTablePtr mBaseLut;
	vtkLookupTablePtr mLookupTable;
	
	//vtkWindowLevelLookupTablePtr mColorLookupTable;
	//vtkPiecewiseFunctionPtr mLowLevel;
	vtkImageDataPtr mBase;
	vtkImageMapToColorsPtr mImageMapToColor;
	double mLLR;
	double mWindow;
	double mLevel;
};

}

#endif /* SSCIMAGELOOKUPTABLE2D_H_ */
