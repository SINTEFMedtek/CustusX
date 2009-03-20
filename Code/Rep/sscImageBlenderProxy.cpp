#include "sscImageBlenderProxy.h"

#include <vtkImageReslice.h>
#include <vtkImageMapToColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageData.h>
#include <vtkImageBlend.h>
#include "sscDataManager.h"

namespace ssc
{

ImageBlenderProxy::ImageBlenderProxy()
{
	mBlender = vtkImageBlendPtr::New();
	mBlender->SetBlendModeToNormal();
	mTreshold = vtkImageThreshold::New();
	//mColorMapper = vtkImageMapToWindowLevelColors::New();
	firstImage = true;
}

ImageBlenderProxy::~ImageBlenderProxy()
{
	
}

void ImageBlenderProxy::createOverlay()
{
	if(mImages.size()<1)
		return;
	vtkImageDataPtr baseImage = mImages[0]->getRefVtkImageData();
	
	mBlender->RemoveAllInputs();
	std::cout<<"There are ("<<mImages.size()<<") to be overlayed"<<std::endl;	
	
	for(unsigned int i = 0; i<mImages.size(); ++i )
	{
		vtkImageDataPtr imageData = mImages[i]->getRefVtkImageData();
		vtkImageReslicePtr resample = vtkImageReslicePtr::New();
		resample->SetInput(imageData);
		resample->SetInterpolationModeToLinear();
		resample->SetInformationInput(baseImage);
		
		vtkImageMapToWindowLevelColorsPtr ColorMapper  = vtkImageMapToWindowLevelColorsPtr::New();	
		ColorMapper->SetInput( resample->GetOutput() );
		//Get the individual color tables
		vtkScalarsToColorsPtr lut =  mImages[i]->lookupTable2D().getLookupTable();
		if(lut)
		{
			std::cout<<"change lut"<<std::endl;
			ColorMapper->SetLookupTable(lut);
			//ColorMapper->SetInput( resample->GetOutput() );	
			ColorMapper->SetWindow(255.0);
			ColorMapper->SetLevel(127.5);
			ColorMapper->Update();
		}
		
		std::cout<<"Number of components "<<ColorMapper->GetOutput()->GetNumberOfScalarComponents()<<std::endl;
		mBlender->SetOpacity(i, mImages[i]->getAlpha() );
		mBlender->AddInput(0, ColorMapper->GetOutput() );
		
	}
}

void  ImageBlenderProxy::sliceFirstTheneBlend()
{
	
}

void  ImageBlenderProxy::updateBlender()
{
	for( int i = 0; i<mBlender->GetNumberOfInputs(); ++i )
	{
		//mColorMapper = vtkImageMapToWindowLevelColorsPtr::New();
		vtkImageMapToWindowLevelColorsPtr ColorMapper = vtkImageMapToWindowLevelColorsPtr::New();	
		
		ColorMapper->SetInput( mBlender->GetInput(i) );	
		vtkLookupTablePtr lut = vtkLookupTable::SafeDownCast( mImages[i]->lookupTable2D().getLookupTable() );
		
		ColorMapper->SetLookupTable( lut );
		ColorMapper->SetWindow(255.0);
		ColorMapper->SetLevel(127.5);

		
		mBlender->SetOpacity(i,0.50 );
		mBlender->Update();
	}
	std::cout<<"har igjen "<<mBlender->GetNumberOfInputs()<<std::endl;
}

void ImageBlenderProxy::updateAlpha()
{
	int blenderSize = mBlender->GetNumberOfInputs();
	for(int i = 0; i<blenderSize; i++)
	{
		std::cout<<"set Alpha for image"<<i<<"Alpha: "<<mImages[i]->getAlpha()<<std::endl;
		mBlender->SetOpacity(i ,  mImages[i]->getAlpha() );
		mBlender->Update();
	}
	std::cout<<"ImageBlenderProxy got signal on Alpha "<<std::endl;
}

void ImageBlenderProxy::volumeBlend()
{
	//vtkImageDataPtr baseImage = mImages[0]->getRefVtkImageData();
	mBlender->RemoveAllInputs();
	
	std::cout<<"There are ("<<mImages.size()<<") to be overlayed"<<std::endl;	
	
	for(unsigned int j = 0; j<mImages.size(); ++j )
	{
//		vtkImageDataPtr imageData = mImages[j]->getRefVtkImageData();
//		vtkImageReslicePtr resample = vtkImageReslicePtr::New();
//		resample->SetInput(imageData);
//		resample->SetInterpolationModeToLinear();
//		resample->SetInformationInput(baseImage);
//		
		
		vtkImageMapToWindowLevelColorsPtr colorMapper = vtkImageMapToWindowLevelColorsPtr::New();
		//colorMapper->SetInput(resample->GetOutput());
		colorMapper->SetInput( mImages[j]->getRefVtkImageData());
		colorMapper->SetWindow(255);
		colorMapper->SetLevel(127.5);
		colorMapper->Update();

		vtkImageDataPtr outColor = colorMapper->GetOutput();
		int* dims = outColor->GetDimensions();
		int tot = dims[0]*dims[1]*dims[2];

	    unsigned char* outBuffer = (unsigned char*)(outColor->GetScalarPointer());
	    for(int i=0; i<tot; i++)
	    {
	      int val = (int)(*outBuffer);
	      outBuffer += 3;
	      (*outBuffer++)=(unsigned char)(val);
	    }   	    	    
		std::cout<<"Number of components "<<colorMapper->GetOutput()->GetNumberOfScalarComponents()<<std::endl;
		mBlender->SetOpacity(j, mImages[j]->getAlpha() );
		mBlender->AddInput ( outColor );
	}
	
}

void  ImageBlenderProxy::addSlice(vtkImageDataPtr slice)
{
	
	if(firstImage)
	{
		mBlender->RemoveAllInputs();
		baseImage = slice;
		firstImage = false;
	}
	vtkImageReslicePtr resample = vtkImageReslicePtr::New();
	resample->SetInput(slice);
	resample->SetInterpolationModeToLinear();
	resample->SetInformationInput(baseImage);
	
	
	mBlender->AddInput(slice);
}

bool ImageBlenderProxy::hasImage(ImagePtr image) const
{	
	return true;
}

void  ImageBlenderProxy::clearImages()
{
	
}

void ImageBlenderProxy::addImage(ImagePtr image)
{
	if (!image)
	{
		return;
	}
	vtkImageDataPtr baseImage = image->getRefVtkImageData();
	std::cout<< "incoming image has ScalarType :"<<baseImage ->GetScalarType()<<std::endl;
	mImages.push_back(image);
}

vtkImageDataPtr ImageBlenderProxy::getOutput()
{
	if(mBlender->GetOutput())
		return mBlender->GetOutput();
	return vtkImageDataPtr();
}

void ImageBlenderProxy::removeImage(ImagePtr image)
{
	std::string uid = image->getUid();
	for(unsigned int i = 0; i<mImages.size(); ++i)	
	{
		if( uid == mImages[i]->getUid())
		{
			mImages.erase(mImages.begin()+i);
		}
	}
	
}	


//----------------------------------------------------------
}//end namespace
//----------------------------------------------------------
