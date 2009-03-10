/*
 * sscImageTF3D.cpp
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#include "sscImageTF3D.h"

#include <vtkImageData.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>

typedef vtkSmartPointer<class vtkWindowLevelLookupTable> vtkWindowLevelLookupTablePtr;

namespace ssc
{

ImageTF3D::ImageTF3D(vtkImageDataPtr base) :
	mOpacityTF(vtkPiecewiseFunctionPtr::New()),
	mColorTF(vtkColorTransferFunctionPtr::New()),	
	mVolumeProperty(vtkVolumePropertyPtr::New()),
	mBase(base)
{
	double max = getScalarMax();
	mLevel = max/2.0;
	mWindow = max;
	mLLR = 0.0;
	
	mColorTF->SetColorSpaceToRGB();
	mOpacityTF->AddPoint(0.0, 0.0);
	mOpacityTF->AddPoint(255, 1.0);

	
	mColorTF->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	mColorTF->AddRGBPoint(255, 1.0, 1.0, 1.0);


//	vtkWindowLevelLookupTablePtr lut = vtkWindowLevelLookupTablePtr::New();
//	int numColors = lut->GetNumberOfTableValues();
//	for ( int i = 0; i < numColors; i++ )
//	{
//		double val = double(i)/(numColors-1);
//		lut->SetTableValue(i, val, val, val, 0);
//	}
//
//	lut->SetWindow(mWindow);
//	lut->SetLevel(mLevel);
//	mLut = lut;
//
//	refreshColorTF();
//	refreshOpacityTF();
}

void ImageTF3D::setOpacityTF(vtkPiecewiseFunctionPtr tf)
{
	mOpacityTF = tf;
}

vtkPiecewiseFunctionPtr ImageTF3D::getOpacityTF()
{
	return mOpacityTF;
}

void ImageTF3D::setColorTF(vtkColorTransferFunctionPtr tf)
{
	mColorTF = tf;
}

vtkColorTransferFunctionPtr ImageTF3D::getColorTF()
{
	std::cout<<"fetching color for 3d head" <<std::endl;
	return mColorTF;
}

/**Set Low Level Reject, meaning the lowest intensity
 * value that will be visible.
 */
void ImageTF3D::setLLR(double val)
{
	mLLR = val;
	refreshOpacityTF();
}

double ImageTF3D::getLLR() const
{
	return mLLR;
}

/**Set Window, i.e. the size of the intensity
 * window that will be visible.
 */
void ImageTF3D::setWindow(double val)
{
	mWindow = val;
	std::cout<<"setWindow, val: "<<val<<std::endl;
	refreshColorTF();
}

double ImageTF3D::getWindow() const
{
	return mWindow;
}

/**Set Level, i.e. the position of the intensity
 * window that will be visible.
 */
void ImageTF3D::setLevel(double val)
{
	std::cout<<"setLevel, val: "<<val<<std::endl;
	mLevel = val;
	refreshColorTF();
}

double ImageTF3D::getLevel() const
{
	return mLevel;
}

/**set a lut that is used as a basis for the color tf.
 *
 */
void ImageTF3D::setLut(vtkLookupTablePtr lut)
{
	std::cout<<"Lut set in 3d property" <<std::endl;
	mLut = lut;
	refreshColorTF();
}

vtkLookupTablePtr ImageTF3D::getLut() const
{
	return mLut;
}

/**Return the maximum intensity value of the underlying dataset.
 * The range of all settings is |0,max>
 *
 */
double ImageTF3D::getScalarMax() const
{
	return 	mBase->GetScalarRange()[1];
}

/**update the color TF according to the
 * mLevel, mWindow and mLut values.
 */
void ImageTF3D::refreshColorTF()
{
	double min = mLevel - ( mWindow / 2.0 );
	double max = mLevel + ( mWindow / 2.0 );

	if (!mLut)
	{
		return;
	}

	mColorTF->RemoveAllPoints();
	int numColors = mLut->GetNumberOfTableValues();
	for ( int i = 0; i < numColors; i++ )
	{
		double* color = mLut->GetTableValue ( i );
		double index = min + double(i) * (max-min) / (numColors-1);
		mColorTF->AddRGBPoint ( index, color[0], color[1], color[2] );
	}
}

/**update the opacity TF according to the mLLR value
 */
void ImageTF3D::refreshOpacityTF()
{
	if (mLLR >= getScalarMax())
	{
		return;
	}

	mOpacityTF->RemoveAllPoints();
	mOpacityTF->AddPoint(0.0, 0.0 );
	mOpacityTF->AddPoint(mLLR, 0.0 );
	mOpacityTF->AddPoint(getScalarMax(), 1.0 );
	mOpacityTF->Update();
}


}
