/*
 * sscImageLookupTable2D.cpp
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#include "sscImageLUT2D.h"
#include <QDir>
#include <QTextStream>
#include <QFile>

#include <vtkImageData.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkPiecewiseFunction.h>
#include <vtkImageMapToColors.h>
#include "sscVector3D.h"

#include <vtkLookupTable.h>
namespace ssc
{

ImageLUT2D::ImageLUT2D(vtkImageDataPtr base) :
	mBase(base)
{
	//dafault Full Range.... or level-it 
	mLevel =  0; //getScalarMax() / 2.0;
	mWindow = getScalarMax();
	mLLR = 0.0;
	mAlpha = 1.0;
	
	mOutputLUT = vtkLookupTablePtr::New();
	
	//make a default system set lookuptable, grayscale...
	vtkLookupTablePtr bwLut= vtkLookupTablePtr::New();
	bwLut->SetTableRange (0, 1);
	bwLut->SetSaturationRange (0, 0);
	bwLut->SetHueRange (0, 0);
	bwLut->SetValueRange (0, 1);
	bwLut->Build();
	this->setBaseLookupTable(bwLut); 
}

void ImageLUT2D::setVtkImageData(vtkImageDataPtr base)
{
	mBase = base;
}

/**set basic lookuptable, to be modified by level/window/llr/alpha
 */
void ImageLUT2D::setBaseLookupTable(vtkLookupTablePtr lut)
{
	if (lut==mBaseLUT)
		return;
	
	mBaseLUT = lut;
	mOutputLUT->DeepCopy(mBaseLUT);
	refreshOutput();
}

vtkLookupTablePtr ImageLUT2D::getOutputLookupTable()
{			
	return mOutputLUT;
}

/**Set Low Level Reject, meaning the lowest intensity
 * value that will be visible.
 */
void ImageLUT2D::setLLR(double val)
{
	if (similar(mLLR, val))
		return;
	mLLR = val;
	refreshOutput();
}

double ImageLUT2D::getLLR() const
{
	return mLLR;
}

void ImageLUT2D::setAlpha(double val)
{
	if (similar(mAlpha, val))
		return;
	mAlpha = val;	
	refreshOutput();
}

double ImageLUT2D::getAlpha() const
{
	return mAlpha;
}

/**Set Window, i.e. the size of the intensity
 * window that will be visible.
 */
void ImageLUT2D::setWindow(double window)
{
	window = std::max(1e-5, window);

	if (similar(mWindow, window))
		return;
	mWindow = window;
	refreshOutput();
}

double ImageLUT2D::getWindow() const
{
	return mWindow;
}

/**Set Level, i.e. the position of the intensity
 * window that will be visible.
 */
void ImageLUT2D::setLevel(double level)
{
	if (similar(mLevel, level))
		return;
	mLevel = level;
	refreshOutput();
}

double ImageLUT2D::getLevel() const
{
	return mLevel;
}

/**Return the maximum intensity value of the underlying dataset.
 * The range of all settings is |0,max>
 */
double ImageLUT2D::getScalarMax() const
{
	return mBase->GetScalarRange()[1];
}

/**rebuild the output lut from all inputs.
 */
void ImageLUT2D::refreshOutput()
{
	double opacity = 0; // ignore alpha, used by blender
	changeOpacity(mLLR, opacity);
	mOutputLUT->SetTableRange(mLevel-mWindow/2.0, mLevel+mWindow/2.0);
	
	emit transferFunctionsChanged();

}

/*this is a version off a llr method on Alpha channel on the lookuptable*/
void ImageLUT2D::changeOpacity(double index_dbl, double opacity)
{
	int index = (int)index_dbl;
	int noValues = mOutputLUT->GetNumberOfTableValues();
	double scale = (getScalarMax()+1)/noValues; 	
	index = (int)(index/scale);	
	
	if (index>noValues)
	{
		std::cout << "could not change opacity. index exceed size of lut ... " << std::endl;
		return;
	}
//	std::cout<<"set the LLR at "<<index<<std::endl;
	
	for ( int i = 0; i < index; i++ )
	{ 
		double rgba[4];
		mOutputLUT->GetTableValue(i, rgba);	
		rgba[ 3 ] = 0.001;
		mOutputLUT->SetTableValue(i, rgba);
	}
	for ( int i = index; i < noValues; i++ )
	{
		double rgba[4];
		mOutputLUT->GetTableValue(i, rgba);
		rgba[ 3 ] = 0.9999;
		mOutputLUT->SetTableValue(i, rgba);
	}
	mOutputLUT->Modified();
}


//---------------------------------------------------------
} // end namespace 
//---------------------------------------------------------





