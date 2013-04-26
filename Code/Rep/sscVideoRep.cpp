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

/*
 * sscRT2DRep.cpp
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */
#include "sscVideoRep.h"

#include "boost/bind.hpp"

#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkImageData.h>
#include <vtkPlaneSource.h>
#include <vtkTransformTextureCoords.h>
#include <vtkTextureMapToPlane.h>
#include <vtkDataSetMapper.h>
#include <vtkTexture.h>
#include <vtkCamera.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include "vtkImageMask.h"
#include <vtkPointData.h>
#include <vtkMatrix4x4.h>
#include <vtkLookupTable.h>
#include <vtkImageThreshold.h>
#include <vtkFloatArray.h>
#include <vtkTextureMapToPlane.h>
#include <vtkCellArray.h>
#include <vtkImageChangeInformation.h>
#include <vtkExtractVOI.h>
#include "sscBoundingBox3D.h"
#include "sscToolManager.h"
#include "sscView.h"
#include "sscTool.h"
#include "sscTypeConversions.h"
#include "sscUltrasoundSectorSource.h"
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscRegistrationTransform.h"
#include "sscLogger.h"

namespace ssc
{


VideoGraphicsPipeline::VideoGraphicsPipeline()
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

VideoGraphicsPipeline::~VideoGraphicsPipeline()
{
}

vtkActorPtr VideoGraphicsPipeline::getActor()
{
	return mPlaneActor;
}

/** Rewire the entire pipeline depending on the configuration,
  * i.e. whether mask or sector or none has been set.
  */
void VideoGraphicsPipeline::setupPipeline()
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

void VideoGraphicsPipeline::setMask(vtkImageDataPtr mask)
{
	if (mInputMask==mask)
		return;
	mInputSector = NULL;
	mInputMask = mask;
	this->setupPipeline();
}

void VideoGraphicsPipeline::setClip(vtkPolyDataPtr sector)
{
	if (mInputSector==sector)
		return;
	mInputMask = NULL;
	mInputSector = sector;
	this->setupPipeline();
}

void VideoGraphicsPipeline::setInputVideo(vtkImageDataPtr video)
{
	if (mInputVideo==video)
		return;
	mInputVideo = video;
	this->setupPipeline();
}

void VideoGraphicsPipeline::setActorUserMatrix(vtkMatrix4x4Ptr rMu)
{
	mPlaneActor->SetUserMatrix(rMu);
}

void VideoGraphicsPipeline::setVisibility(bool visible)
{
	mPlaneActor->SetVisibility(visible);
	mPlaneActor->Modified();
}

void VideoGraphicsPipeline::update()
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

void VideoGraphicsPipeline::connectVideoImageToPipeline()
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

void VideoGraphicsPipeline::updatePlaneSourceBounds()
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

void VideoGraphicsPipeline::updateLUT()
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
void VideoGraphicsPipeline::setLookupTable()
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

bool VideoGraphicsPipeline::inputImageIsEmpty()
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

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

VideoGraphics::VideoGraphics(bool useMaskFilter)
{
	mClipToSector = true;
	mPipeline.reset(new VideoGraphicsPipeline());
	mShowInToolSpace = true;
	mImage = ssc::ImagePtr();
}

VideoGraphics::~VideoGraphics()
{
}

void VideoGraphics::setShowInToolSpace(bool on)
{
	mShowInToolSpace = on;
}

vtkActorPtr VideoGraphics::getActor()
{
	return mPipeline->getActor();
}

ToolPtr VideoGraphics::getTool()
{
	return mTool;
}

ssc::ProbeSector VideoGraphics::getProbeData()
{
	return mProbeData;
}

void VideoGraphics::setTool(ToolPtr tool)
{
	if (tool==mTool)
		return;

	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
		disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));
	}

	// accept only tool with a probe sector
	if (tool && tool->getProbeSector().getType()!=ssc::ProbeData::tNONE)
	{
		mTool = tool;
	}

	// setup new
	if (mTool )
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
		connect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));
	}

	this->probeSectorChanged();

	if(mTool && mImage)
		mImage->setName(mTool->getName());
}

void VideoGraphics::setClipToSector(bool on)
{
	mClipToSector = on;
	this->probeSectorChanged();
}

void VideoGraphics::probeSectorChanged()
{
	if (!mTool)
		return;

	mProbeData.setData(mTool->getProbeSector());
	if (mClipToSector)
	{
		mPipeline->setClip(mProbeData.getSector());
	}
	else
	{
		mPipeline->setClip(NULL);
	}
	this->receiveTransforms(mTool->get_prMt(), 0);

	mPipeline->update();
}


void VideoGraphics::setRealtimeStream(VideoSourcePtr data)
{
	//Don't do anything if data is unchanged
	if (mData == data)
		return;
	if (mData)
	{
		disconnect(mData.get(), SIGNAL(newFrame()), this, SLOT(newDataSlot()));
		mPipeline->setInputVideo(NULL);
	}

	mData = data;

	if (mData)
	{
		connect(mData.get(), SIGNAL(newFrame()), this, SLOT(newDataSlot()));
		mPipeline->setInputVideo(mData->getVtkImageData());

		//Only add image in dataManager once
		mImage = dataManager()->getImage("4D US");
		if(!mImage)
		{
//			mImage = dataManager()->createImage(mDataRedirecter->GetOutput(), "4D US", mData->getName());
//			ssc::dataManager()->loadData(boost::dynamic_pointer_cast<ssc::Data>(mImage));//Uncomment to test unstable 4D US
		}
	}

	this->newDataSlot();
}

void VideoGraphics::receiveTransforms(Transform3D prMt, double timestamp)
{
	if (!mShowInToolSpace)
		return;
	Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
	Transform3D tMu = mProbeData.get_tMu();
	Transform3D rMu = rMpr * prMt * tMu;
	mPipeline->setActorUserMatrix(rMu.getVtkMatrix());

	//TODO: Set correct position and orientation on mImage
	//std::cout << "rMu: " << rMu << std::endl;
	if (mImage)
	{
		mImage->get_rMd_History()->setRegistration(rMu);
	}
}

void VideoGraphics::receiveVisible(bool visible)
{

}

//void VideoGraphics::checkDataIntegrity()
//{
//	if (!mData || !mTool)
//		return;

//	std::cout << "probe sector " << mTool->getUid() << " " << streamXml2String(mTool->getProbeSector()) << std::endl;
//	DoubleBoundingBox3D bounds_poly_u(mProbeData.getSector()->GetBounds());
//	DoubleBoundingBox3D bounds_poly_v = transform(mProbeData.get_uMv().inv(), bounds_poly_u);
//	DoubleBoundingBox3D bounds(mDataRedirecter->GetOutput()->GetBounds());
//	DoubleBoundingBox3D extent(mDataRedirecter->GetOutput()->GetExtent());
//	std::cout << "poly_u bounds: " << bounds_poly_u << std::endl;
//	std::cout << "poly_v bounds: " << bounds_poly_v << std::endl;
//	std::cout << "img bounds: " << bounds << std::endl;
//	std::cout << "img extent: " << extent << std::endl;
//	std::cout << "img spacing: " << Vector3D(mDataRedirecter->GetOutput()->GetSpacing()) << std::endl;

////  mDataRedirecter->GetOutput()->Print(std::cout);
//}

void VideoGraphics::newDataSlot()
{
	if (!mData || !mData->validData())
	{
		mPipeline->setVisibility(false);
		emit newData();
		return;
	}

	mPipeline->update();

	if (mImage)
	{
		mImage->setVtkImageData(mData->getVtkImageData());//Update pointer to 4D image
	}

	bool visible = mData->validData();
	if (mShowInToolSpace)
		visible = visible && mTool && mTool->getVisible();
	mPipeline->setVisibility(visible);

	emit newData();
}

//---------------------------------------------------------
//---------------------------------------------------------


VideoFixedPlaneRep::VideoFixedPlaneRep(const QString& uid, const QString& name) :
	ssc::RepImpl(uid, name)
{
	mRTGraphics.reset(new VideoGraphics());
	connect(mRTGraphics.get(), SIGNAL(newData()), this, SLOT(newDataSlot()));
	mRTGraphics->setShowInToolSpace(false);
	mRTGraphics->setClipToSector(false);

	mInfoText.reset(new ssc::TextDisplay("", Vector3D(1.0, 0.8, 0.0), 16));
	mInfoText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	mInfoText->setCentered();
	mInfoText->setPosition(0.5, 0.05);

	mStatusText.reset(new ssc::TextDisplay("", Vector3D(1.0, 0.8, 0.0), 20));
	mStatusText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	mStatusText->setCentered();
	mStatusText->setPosition(0.5, 0.5);
	mStatusText->updateText("Not Connected");

	mProbeOrigin.reset(new GraphicalPolyData3D());
	mProbeOrigin->setColor(ssc::Vector3D(1, 165.0/255.0, 0)); // orange
	mProbeSector.reset(new GraphicalPolyData3D());
	mProbeSector->setColor(ssc::Vector3D(1, 165.0/255.0, 0)); // orange
	mProbeClipRect.reset(new GraphicalPolyData3D());
	mProbeClipRect->setColor(ssc::Vector3D(1, 0.9, 0)); // yellow

	mViewportListener.reset(new ViewportListener());
	mViewportListener->setCallback(boost::bind(&VideoFixedPlaneRep::setCamera, this));
}

VideoFixedPlaneRep::~VideoFixedPlaneRep()
{
}

void VideoFixedPlaneRep::setShowSector(bool on)
{
	mShowSector = on;
	this->updateSector();
}

bool VideoFixedPlaneRep::getShowSector() const
{
	return mShowSector;
}

void VideoFixedPlaneRep::updateSector()
{
	bool show = mTool && this->getShowSector() && mTool->getProbeSector().getType()!=ssc::ProbeData::tNONE;

	mProbeOrigin->getActor()->SetVisibility(show);
	mProbeSector->getActor()->SetVisibility(show);
	mProbeClipRect->getActor()->SetVisibility(show);
	if (!show)
		return;

	mProbeData.setData(mTool->getProbeSector());

	mProbeOrigin->setData(mProbeData.getOriginPolyData());
	mProbeSector->setData(mProbeData.getSectorSectorOnlyLinesOnly());
	mProbeClipRect->setData(mProbeData.getClipRectLinesOnly());
}

void VideoFixedPlaneRep::setTool(ToolPtr tool)
{
	mRTGraphics->setTool(tool);
	mTool = tool;
}

void VideoFixedPlaneRep::setRealtimeStream(VideoSourcePtr data)
{
	mData = data;
	mRTGraphics->setRealtimeStream(data);
}

void VideoFixedPlaneRep::newDataSlot()
{
	if (!mData)
		return;

	mInfoText->updateText(mData->getInfoString());
	mStatusText->updateText(mData->getStatusString());
	mStatusText->getActor()->SetVisibility(!mData->validData());
	this->setCamera();
	this->updateSector();
}

/**We need this here, even if it belongs in singlelayout.
 * Reason: must call setcamera after last change of size of plane actor.
 * TODO fix it.
 */
void VideoFixedPlaneRep::setCamera()
{
	if (!mRenderer)
		return;
	mViewportListener->stopListen();
	vtkCamera* camera = mRenderer->GetActiveCamera();
	camera->ParallelProjectionOn();
	mRenderer->ResetCamera();

	DoubleBoundingBox3D bounds(mRTGraphics->getActor()->GetBounds());
	if (ssc::similar(bounds.range()[0], 0.0) || ssc::similar(bounds.range()[1], 0.0))
		return;

	double* vpRange = mRenderer->GetAspect();

	double vw = vpRange[0];
	double vh = vpRange[1];

	double w = bounds.range()[0];
	double h = bounds.range()[1];

	double scale = 1;
	double w_vp = vh * (w/h); // width of image in viewport space
	if (w_vp > vw) // if image too wide: reduce scale
		scale = w_vp/vw;

	camera->SetParallelScale(h/2*scale*1.01); // exactly fill the viewport height
	mViewportListener->startListen(mRenderer);
}


void VideoFixedPlaneRep::addRepActorsToViewRenderer(ssc::View* view)
{
	mView = view;
	mRenderer = view->getRenderer();
	mViewportListener->startListen(mRenderer);

	view->getRenderer()->AddActor(mRTGraphics->getActor());
	view->getRenderer()->AddActor(mInfoText->getActor());
	view->getRenderer()->AddActor(mStatusText->getActor());

	mProbeClipRect->setRenderer(view->getRenderer());
	mProbeOrigin->setRenderer(view->getRenderer());
	mProbeSector->setRenderer(view->getRenderer());
}

void VideoFixedPlaneRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	mRenderer = vtkRendererPtr();
	view->getRenderer()->RemoveActor(mRTGraphics->getActor());
	view->getRenderer()->RemoveActor(mInfoText->getActor());
	view->getRenderer()->RemoveActor(mStatusText->getActor());
	mProbeOrigin->setRenderer(NULL);
	mProbeSector->setRenderer(NULL);
	mProbeClipRect->setRenderer(NULL);

	mViewportListener->stopListen();
}

} // namespace ssc
