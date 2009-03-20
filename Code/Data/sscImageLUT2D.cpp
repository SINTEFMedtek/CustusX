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
#include <vtkLookupTable.h>
#include <vtkPiecewiseFunction.h>
#include "sscVector3D.h"

namespace ssc
{

ImageLUT2D::ImageLUT2D(vtkImageDataPtr base) :
	mBase(base)
{
	mLevel = getScalarMax() / 2.0;
	mWindow = getScalarMax();
	mLLR = 0.0;
	std::cout<< "-- ImageLUT2D --"<<std::endl;
	std::cout<<" default windowlevel  ["<<mLevel<<"],["<<mWindow<<"]"<<std::endl;
	
	//make a default system set lookuptable, Default my be gryscale...
	mLookupTable = vtkLookupTablePtr::New();
	mLookupTable ->SetTableRange(0,65536);
	mLookupTable->SetHueRange(0.0, 0.0);	
	mLookupTable->SetSaturationRange(1.0, 1.0);	
	mLookupTable->SetValueRange(0.0, 1.0);
	mLookupTable->Build();
	
}
//set lookupTable, generated from file only froms DataManagerImpl::setViewControlData
void ImageLUT2D::setLookupTable(vtkLookupTablePtr lut)
{
	std::cout<<"Got image lookup tabel "<<std::endl;
	mLookupTable = lut;
	mLookupTable->SetTableRange(mLevel,mWindow); 
	
	//setWindow(getWindow()+1);
	//setLevel(getLevel()+1);
	//printToFile();
}

vtkLookupTablePtr ImageLUT2D::getLookupTable()
{		
	std::cout<<"Image LUT2D, fetch lookuptable"<<std::endl;	
	return mLookupTable;
}

void ImageLUT2D::printToFile()
{
	ofstream myfile;
	myfile.open ("/Data/Logs/example.txt");
	int tableSize = mLookupTable->GetNumberOfTableValues();
	for(int i = 0; i<tableSize; ++i )
	{
		double rgba[4];
		mLookupTable->GetTableValue(i,rgba);
		//stream << "i("<<i<<"), ["<< rgba[0] <<", "<<rgba[1]<<","<<rgba[2]<<","<<rgba[3]<< "]\n";
		myfile << "i("<<i<<"), R["<< rgba[0] <<"] G["<<rgba[1]<<"] B["<<rgba[2]<<"] A["<<rgba[3]<< "]\n";	
	}	
	myfile.close();	
}
void ImageLUT2D::addNewColorLut()
{
	
}
void ImageLUT2D::addNewColor(QColor color)
{
	double value = color.value()/255.0;
	double hue = color.hue()/255.0;
	double saturation = color.saturation()/255.0;
	
//	std::cout<<"Got new color:"<<std::endl;
//	std::cout<<"hue: "<<hue<<"\nsaturation: "<<saturation<<"\nvalue: "<<value<<std::endl;
//	
//	std::cout<<"---------------------"<<std::endl;
	
	mLookupTable->SetSaturationRange(0, saturation);	
	mLookupTable->SetHueRange(0.0, hue);
	mLookupTable->SetValueRange(0.0, value);
	
	
	//mLookupTable->Build();
		
}
void ImageLUT2D::setAlphaRange(double alpha)
{
	std::cout<<"alpha range: [0,"<<alpha<<"]"<<std::endl;
	//mLookupTable->SetAlphaRange(0.0 ,alpha);
	
}
/**Set Low Level Reject, meaning the lowest intensity
 * value that will be visible.
 */
void ImageLUT2D::setLLR(double val)
{
	if (similar(mLLR, val))
		return;
	
	mLLR = val;
	//mLookupTable->SetAlphaRange(mLLR , 1.0);
	//->Build();
	//changeOpacity(mLLR, 0);
	changeOpacityForAll(mLLR);
	
	std::cout<<"alpha range: [0,"<<mLLR<<"]"<<std::endl;
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
	if (similar(mWindow, window))
		return;

	if (window < 1e-5)
	{
		window = 1e-5; 
	}
	mWindow = window;
	std::cout<<"setWindow: WindowWidth (:"<<mWindow<<"), WindowCenter ("<<mLevel<<")"<<std::endl;
	std::cout<<"new range:["<<mLevel-mWindow/2.0<<", "<< mLevel+mWindow/2.0<<"]"<<std::endl;
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
	
	std::cout<<"setLevel: WindowWidth (:"<<mWindow<<"), WindowCenter ("<<mLevel<<")"<<std::endl;
	std::cout<<"new range:["<<mLevel-mWindow/2.0<<", "<< mLevel+mWindow/2.0<<"]"<<std::endl;
	mLookupTable->SetTableRange(mLevel-mWindow/2.0, mLevel+mWindow/2.0);
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

//Gets the number of color in the lut
void ImageLUT2D::changeOpacityForAll(double opacity )
{
	int noValues = mLookupTable->GetNumberOfTableValues ();
	std::cout << " noValues" <<noValues<< std::endl;
	//prints
	double rgba[ 4 ];
	for ( int i = 0;i < noValues;i++ )
	{
		mLookupTable->GetTableValue ( i, rgba );
		rgba[ 3 ] = opacity;
		mLookupTable->SetTableValue ( i, rgba );
	}
	mLookupTable->Modified();  // need to call modiefied, since LookupTableProperty seems to be unchanged so no widget-updat is executed
}

void ImageLUT2D::changeOpacity(int index, double opacity)
{
	int noValues = mLookupTable->GetNumberOfTableValues();
	std::cout << " noValues" <<noValues<< std::endl;
	
	if (index>noValues)
	{
		std::cout << "could not change opacity. index exceed size of lut ... " << std::endl;
		return;
	}
	double rgba[ 4 ];
	mLookupTable->GetTableValue(index, rgba);
	rgba[ 3 ] = opacity;
	mLookupTable->SetTableValue(index, rgba);
	mLookupTable->Modified(); // need to call modiefied, since LookupTableProperty seems to be unchanged so no widget-updat is executed
}


//---------------------------------------------------------
} // end namespace 
//---------------------------------------------------------





