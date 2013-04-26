// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscVideoGraphics.h"

#include <vtkActor.h>
#include <vtkImageData.h>
#include <vtkPlaneSource.h>
#include <vtkTransformTextureCoords.h>
#include <vtkTextureMapToPlane.h>
#include <vtkDataSetMapper.h>
#include <vtkTexture.h>
#include <vtkProperty.h>
#include <vtkImageMask.h>
#include <vtkPointData.h>
#include <vtkMatrix4x4.h>
#include <vtkLookupTable.h>
#include <vtkImageThreshold.h>
#include <vtkImageChangeInformation.h>
#include <vtkExtractVOI.h>

#include "sscUltrasoundSectorSource.h"
#include "sscBoundingBox3D.h"

namespace ssc
{

VideoGraphics::VideoGraphics()
{
	mPlaneActor = vtkActorPtr::New();
	mPlaneSource = vtkPlaneSourcePtr::New();

	mDataRedirecter = vtkImageChangeInformationPtr::New();
	mUSSource = UltrasoundSectorSourcePtr::New();

	// set a filter that map all zeros in the input to ones. This enables us to
	// use zero as a special transparency value, to be used in masking.
	mMapZeroToOne = vtkImageThresholdPtr::New();
	mMapZeroToOne->ThresholdByLower(1.0);
	mMapZeroToOne->SetInValue(1);
	mMapZeroToOne->SetReplaceIn(true);

	// set the filter that applies a mask to the stream data
	mMaskFilter = vtkImageMaskPtr::New();
	mMaskFilter->SetMaskInput(vtkImageDataPtr());
	mMaskFilter->SetMaskedOutputValue(0.0);

	// generate texture coords for mPlaneSource
	mTextureMapToPlane = vtkTextureMapToPlanePtr::New();

	mTransformTextureCoords = vtkTransformTextureCoordsPtr::New();
	mTransformTextureCoords->SetOrigin( 0, 0.5, 0);
	mTransformTextureCoords->SetScale( 1, 1, 0);
	mTransformTextureCoords->FlipROn(); // flip around axis

	mTexture = vtkTexturePtr::New();
	mTexture->RepeatOff();

	mDataSetMapper = vtkDataSetMapperPtr::New();

	mPlaneActor->SetTexture(mTexture);
	mPlaneActor->SetMapper(mDataSetMapper);
	mPlaneActor->SetVisibility(false);
	mPlaneActor->GetProperty()->LightingOff();	// Turning off lighting to remove shadow effects (Fix for #644: 2D ultrasound in 3D scene was too dark)
}

VideoGraphics::~VideoGraphics()
{
}

vtkActorPtr VideoGraphics::getActor()
{
	return mPlaneActor;
}

/** Rewire the entire pipeline depending on the configuration,
  * i.e. whether mask or sector or none has been set.
  */
void VideoGraphics::setupPipeline()
{
	if (!mInputVideo)
	{
		mTexture->SetInput(NULL);
		return;
	}

	if (mInputMask)
	{
		mTextureMapToPlane->SetInput(mPlaneSource->GetOutput());
		mTransformTextureCoords->SetInput(mTextureMapToPlane->GetOutput() );
		mDataSetMapper->SetInput(mTransformTextureCoords->GetOutput() );

		mMaskFilter->SetMaskInput(mInputMask);
		mMapZeroToOne->SetInput(mDataRedirecter->GetOutput());
		mMaskFilter->SetImageInput(mMapZeroToOne->GetOutput());
		mTexture->SetInput(mMaskFilter->GetOutput());
	}
	else if (mInputSector)
	{
		mUSSource->setProbeSector(mInputSector);
		mTransformTextureCoords->SetInput(mUSSource->GetOutput() );
		mDataSetMapper->SetInput(mTransformTextureCoords->GetOutput() );

		mTexture->SetInput(mDataRedirecter->GetOutput());
	}
	else
	{
		mTextureMapToPlane->SetInput(mPlaneSource->GetOutput());
		mTransformTextureCoords->SetInput(mTextureMapToPlane->GetOutput() );
		mDataSetMapper->SetInput(mTransformTextureCoords->GetOutput() );

		mTexture->SetInput(mDataRedirecter->GetOutput());
	}

	this->setLookupTable();
	mPlaneActor->SetTexture(mTexture);
	mPlaneActor->SetMapper(mDataSetMapper);
}

void VideoGraphics::setMask(vtkImageDataPtr mask)
{
	if (mInputMask==mask)
		return;
	mInputSector = NULL;
	mInputMask = mask;
	this->setupPipeline();
}

void VideoGraphics::setClip(vtkPolyDataPtr sector)
{
	if (mInputSector==sector)
		return;
	mInputMask = NULL;
	mInputSector = sector;
	this->setupPipeline();
}

void VideoGraphics::setInputVideo(vtkImageDataPtr video)
{
	if (mInputVideo==video)
		return;
	mInputVideo = video;
	this->setupPipeline();
}

void VideoGraphics::setActorUserMatrix(vtkMatrix4x4Ptr rMu)
{
	mPlaneActor->SetUserMatrix(rMu);
}

void VideoGraphics::setVisibility(bool visible)
{
	mPlaneActor->SetVisibility(visible);
	mPlaneActor->Modified();
}

void VideoGraphics::update()
{
	if (this->inputImageIsEmpty())
	{
		this->setVisibility(false);
		return;
	}

	this->connectVideoImageToPipeline();
	this->updateLUT();
	this->updatePlaneSourceBounds();

	mPlaneActor->Modified();
}

void VideoGraphics::connectVideoImageToPipeline()
{
	if (mInputVideo == NULL)
	{
		mTexture->SetInput(NULL); // TODO trouble - will destroy the pipeline
		return;
	}

	//Check if 3D volume. If so, only use middle frame
	int* extent = mInputVideo->GetExtent();
	if(extent[5] - extent[4] > 0)
	{
		int slice = floor(extent[4]+0.5f*(extent[5]-extent[4]));
		if (slice < 0) slice = 0;
//		std::cout << "Got 3D volume, showing middle slice: " << slice << std::endl;
		vtkSmartPointer<vtkExtractVOI> extractVOI = vtkSmartPointer<vtkExtractVOI>::New();
		extractVOI->SetInput(mInputVideo);
		extractVOI->SetVOI(extent[0], extent[1], extent[2], extent[3], slice, slice);
		extractVOI->Update();
		mDataRedirecter->SetInput(extractVOI->GetOutput());
	}
	else //2D
	{
		mDataRedirecter->SetInput(mInputVideo);
	}

	mDataRedirecter->UpdateWholeExtent(); // important! syncs update extent to whole extent
	mDataRedirecter->GetOutput()->Update(); //???
}

void VideoGraphics::updatePlaneSourceBounds()
{
	// set the planesource where we have no probedata.
	// TODO dont do this when planesource is not part of pipeline.
	DoubleBoundingBox3D bounds(mDataRedirecter->GetOutput()->GetBounds());
	if (!ssc::similar(bounds.range()[0], 0.0) || !ssc::similar(bounds.range()[1], 0.0))
	{
		mPlaneSource->SetOrigin(bounds.corner(0,0,0).begin());
		mPlaneSource->SetPoint1(bounds.corner(1,0,0).begin());
		mPlaneSource->SetPoint2(bounds.corner(0,1,0).begin());
		mPlaneSource->GetOutput()->GetPointData()->Modified();
		mPlaneSource->GetOutput()->Modified();
	}
}

void VideoGraphics::updateLUT()
{
	this->setLookupTable();
	mTexture->SetLookupTable(mLUT);

	// apply a lut only if the input data is monochrome
	int numComp = mDataRedirecter->GetOutput()->GetNumberOfScalarComponents();
	bool is8bit = mDataRedirecter->GetOutput()->GetScalarType()==VTK_UNSIGNED_CHAR;
	if (numComp==1)
	{
		double srange[2];
		if (is8bit)
		{
			srange[0] = 0;
			srange[1] = 255;
		}
		else
		{
			mDataRedirecter->GetOutput()->GetScalarRange(srange);
		}

		mTexture->GetLookupTable()->SetRange(srange[0], srange[1]);
		mTexture->MapColorScalarsThroughLookupTableOn();
	}
	else
	{
		mTexture->MapColorScalarsThroughLookupTableOff();
	}
}

/** Create a lut that sets zeros to transparent and applies a linear grayscale to the rest.
 */
void VideoGraphics::setLookupTable()
{
	// applies only to mask:
	// Create a lut of size at least equal to the data range. Set the tableRange[0] to zero.
	// This will force input zero to be mapped onto the first table value (the transparent one),
	// and inputs [1, -> > is mapped to larger values, not transparent.
	// In order to create a window-level function, manually build a table.

	//make a default system set lookuptable, grayscale...
	vtkLookupTablePtr lut = vtkLookupTablePtr::New();
	lut->SetNumberOfTableValues(1000); // large enough to give resolution even for ct images.
	//lut->SetTableRange (0, 1024); // the window of the input
	lut->SetTableRange (0, 255); // the window of the input - must be reset according to data
	lut->SetSaturationRange (0, 0);
	lut->SetHueRange (0, 0);
	lut->SetValueRange (0, 1);
	lut->Build();

	if (mInputMask)
	{
		lut->SetTableValue(0, 0, 0, 0, 0); // set the lowest value to transparent. This will make the masked values transparent, but nothing else
	}

	lut->Modified();
	mLUT = lut;
}

bool VideoGraphics::inputImageIsEmpty()
{
	if (mInputVideo == NULL)
		return true;
	mInputVideo->Update();
	//Don't do anything if we get an empty image
	int* dim = mInputVideo->GetDimensions();
	if(dim[0] == 0 || dim[1] == 0)
		return true;

	return false;
}

} // namespace ssc
