/*
 * sscImageLookupTable2D.h
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#ifndef SSCIMAGELOOKUPTABLE2D_H_
#define SSCIMAGELOOKUPTABLE2D_H_

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkScalarsToColors> vtkScalarsToColorsPtr;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace ssc
{

/**Handler for the lookup table used in 2d image slices.
 * Used by Image.
 */
class ImageLUT2D
{
public:
	ImageLUT2D(vtkImageDataPtr base);

	void setLookupTable(vtkScalarsToColorsPtr lut);
	vtkScalarsToColorsPtr getLookupTable();

	void setLLR(double val);
	double getLLR() const;
	void setWindow(double val);
	double getWindow() const;
	void setLevel(double val);
	double getLevel() const;
	double getScalarMax() const;
private:
	vtkScalarsToColorsPtr mLut;
	vtkImageDataPtr mBase;
	double mLLR;
	double mWindow;
	double mLevel;
};

}

#endif /* SSCIMAGELOOKUPTABLE2D_H_ */
