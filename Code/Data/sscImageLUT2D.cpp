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
	
	std::cout<< "--[ ImageLUT2D ]--"<<std::endl;	
	std::cout<<  "window "<<mWindow<<std::endl;
	std::cout<<  "level "<<mLevel<<std::endl;
	
	mImageMapToColor = vtkImageMapToColorsPtr::New();
	mLookupTable = vtkLookupTablePtr::New();
	
	//make a default system set lookuptable, gryscale...
	vtkLookupTablePtr bwLut= vtkLookupTablePtr::New();
	bwLut->SetTableRange (0, 1);
	bwLut->SetSaturationRange (0, 0);
	bwLut->SetHueRange (0, 0);
	bwLut->SetValueRange (0, 1);
	bwLut->Build();
	this->setLookupTable(bwLut); 
}
//set lookupTable, generated from file only froms DataManagerImpl::setViewControlData
void ImageLUT2D::setLookupTable(vtkLookupTablePtr lut)
{
	mLookupTable = lut;
	mLookupTable->SetTableRange(mLevel,mWindow);
	mImageMapToColor->SetLookupTable(mLookupTable);
}

void ImageLUT2D::setImportedLut(vtkLookupTablePtr lut)
{
	mBaseLut = lut;
	this->setLookupTable(mBaseLut);
}
vtkImageMapToColorsPtr ImageLUT2D::getColorMap()
{
	return mImageMapToColor;
}
void ImageLUT2D::setTable(vtkUnsignedCharArray* table)
{
	mLookupTable->SetTable(table);
}

vtkLookupTablePtr ImageLUT2D::getLookupTable()
{			
	return mLookupTable;
}

//debug method.. will be removed later 
void ImageLUT2D::printToFile()
{
	ofstream myfile;
	myfile.open ("/Data/Logs/example.txt");
	int tableSize = mLookupTable->GetNumberOfTableValues();
	for(int i = 0; i<tableSize; ++i )
	{
		double rgba[4];
		mLookupTable->GetTableValue(i,rgba);
		myfile << "i("<<i<<"), R["<< rgba[0] <<"] G["<<rgba[1]<<"] B["<<rgba[2]<<"] A["<<rgba[3]<< "]\n";	
	}	
	myfile.close();	
}
	
void ImageLUT2D::setAlphaRange(double alpha)
{
	int scale = (int)(this->getScalarMax() + 1.0 / mLookupTable->GetTableRange()[1]);
	int r = (int)alpha / scale;
	mLookupTable->SetAlphaRange(0.0 ,r);
	mLookupTable->SetAlphaRange(r , 1.0);
}

/**Set Low Level Reject, meaning the lowest intensity
 * value that will be visible.
 */
void ImageLUT2D::setLLR(double val)
{
	if (similar(mLLR, val))
		return;
	mLLR = val;	
	changeOpacity(mLLR, 0);
	//changeOpacityForAll(mLLR);
}

double ImageLUT2D::getLLR() const
{
	return mLLR;
}

/**Set Window, i.e. the size of the intensity
 * window that will be visible.
 */
void ImageLUT2D::setWindow(double window)
{
	//std::cout<<"setWindow "<<window<<std::endl;
	if (similar(mWindow, window))
		return;

	if (window < 1e-5)
	{
		window = 1e-5; 
	}
	mWindow = window;
	mLookupTable->SetTableRange(mLevel-mWindow/2.0, mLevel+mWindow/2.0); 
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
	mLookupTable->SetTableRange(mLevel-mWindow/2.0, mLevel+mWindow/2.0);
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

//set the a overall opacity.
void ImageLUT2D::changeOpacityForAll(double opacity )
{
	int noValues = mLookupTable->GetNumberOfTableValues();	
	double rgba[4];
	for ( int i = 0; i <noValues; i++ )
	{
		mLookupTable->GetTableValue (i, rgba);
		rgba[3] = opacity;
		mLookupTable->SetTableValue (i, rgba);
	}
	mLookupTable->Modified();
}

/*this is a version off a llr method on Alpha channel on the lookuptable*/
void ImageLUT2D::changeOpacity(double index_dbl, double opacity)
{
	int index = (int)index_dbl;
	int noValues = mLookupTable->GetNumberOfTableValues();
	double scale = (getScalarMax()+1)/noValues; 	
	index = (int)(index/scale);	
	
	if (index>noValues)
	{
		std::cout << "could not change opacity. index exceed size of lut ... " << std::endl;
		return;
	}
	std::cout<<"set the LLR at "<<index<<std::endl;
	
	for ( int i = 0; i < index; i++ )
	{ 
		double rgba[4];
		mLookupTable->GetTableValue(i, rgba);	
		rgba[ 3 ] = 0.001;
		mLookupTable->SetTableValue(i, rgba);
	}
	for ( int i = index; i < noValues; i++ )
	{
		double rgba[4];
		mLookupTable->GetTableValue(i, rgba);
		rgba[ 3 ] = 0.9999;
		mLookupTable->SetTableValue(i, rgba);
	}
	mLookupTable->Modified();
}


//---------------------------------------------------------
} // end namespace 
//---------------------------------------------------------





