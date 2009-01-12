/*
 * sscImageLookupTable2D.cpp
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#include "sscImageLUT2D.h"

#include <vtkImageData.h>
#include <vtkWindowLevelLookupTable.h>
typedef vtkSmartPointer<class vtkWindowLevelLookupTable> vtkWindowLevelLookupTablePtr;



namespace ssc
{

ImageLUT2D::ImageLUT2D(vtkImageDataPtr base) :
	mBase(base)
{
	mLevel = getScalarMax() / 2.0;
	mWindow = getScalarMax();
	mLLR = 0.0;

	vtkWindowLevelLookupTablePtr lut = vtkWindowLevelLookupTablePtr::New();
	lut->SetWindow(mWindow);
	lut->SetLevel(mLevel);
	lut->Build();
	mLut = lut;
}

void ImageLUT2D::setLookupTable(vtkScalarsToColorsPtr lut)
{
	mLut = lut;
}

vtkScalarsToColorsPtr ImageLUT2D::getLookupTable()
{
	return mLut;
}

/**Set Low Level Reject, meaning the lowest intensity
 * value that will be visible.
 */
void ImageLUT2D::setLLR(double val)
{
	mLLR = val;
	// TODO not implemented
}

double ImageLUT2D::getLLR() const
{
	return mLLR;
}

/**Set Window, i.e. the size of the intensity
 * window that will be visible.
 */
void ImageLUT2D::setWindow(double val)
{
	mWindow = val;
	vtkWindowLevelLookupTable *lut = vtkWindowLevelLookupTable::SafeDownCast(mLut);
	if (lut)
	{
		lut->SetWindow(mWindow);
	}
}

double ImageLUT2D::getWindow() const
{
	return mWindow;
}

/**Set Level, i.e. the position of the intensity
 * window that will be visible.
 */
void ImageLUT2D::setLevel(double val)
{
	mLevel = val;
	vtkWindowLevelLookupTable *lut = vtkWindowLevelLookupTable::SafeDownCast(mLut);
	std::cout << "set level" << std::endl;
	if (lut)
	{
		std::cout << "actual set level " << val << std::endl;
		lut->SetLevel(mLevel);
	}
}

double ImageLUT2D::getLevel() const
{
	return mLevel;
}

/**Return the maximum intensity value of the underlying dataset.
 * The range of all settings is |0,max>
 *
 */
double ImageLUT2D::getScalarMax() const
{
	return mBase->GetScalarRange()[1];
}

}
