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

#include "sscVideoSourceGraphics.h"

#include <vtkImageData.h>
#include <vtkPolyData.h>

#include "sscToolManager.h"
#include "sscTool.h"
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscRegistrationTransform.h"
#include "sscVideoGraphics.h"
#include "sscVideoSource.h"

namespace ssc
{

VideoSourceGraphics::VideoSourceGraphics(bool useMaskFilter)
{
	mClipToSector = true;
	mPipeline.reset(new VideoGraphics());
	mShowInToolSpace = true;
	mImage = ssc::ImagePtr();
}

VideoSourceGraphics::~VideoSourceGraphics()
{
}

void VideoSourceGraphics::setShowInToolSpace(bool on)
{
	mShowInToolSpace = on;
}

vtkActorPtr VideoSourceGraphics::getActor()
{
	return mPipeline->getActor();
}

ToolPtr VideoSourceGraphics::getTool()
{
	return mTool;
}

ssc::ProbeSector VideoSourceGraphics::getProbeData()
{
	return mProbeData;
}

void VideoSourceGraphics::setTool(ToolPtr tool)
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

void VideoSourceGraphics::setClipToSector(bool on)
{
	mClipToSector = on;
	this->probeSectorChanged();
}

void VideoSourceGraphics::probeSectorChanged()
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


void VideoSourceGraphics::setRealtimeStream(VideoSourcePtr data)
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

void VideoSourceGraphics::receiveTransforms(Transform3D prMt, double timestamp)
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

void VideoSourceGraphics::receiveVisible(bool visible)
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

void VideoSourceGraphics::newDataSlot()
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

} // namespace ssc
