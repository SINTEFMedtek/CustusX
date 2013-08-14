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

#include "sscProgressiveLODVolumetricRep.h"

#include <boost/lexical_cast.hpp>
#include <QtCore>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkAssembly.h>

#include "sscView.h"
#include "sscVolumetricRep.h"
#include "sscTypeConversions.h"

namespace ssc
{
/**Helper class for rendering a time-consuming ssc::Rep
 * in another thread. Construct and start the thread,
 * connect to finished() and use the result in the finished()
 * slot.
 *
 */
class VolumetricRepThreadedRenderer : public QThread
{
public:
	VolumetricRepThreadedRenderer(ssc::VolumetricRepPtr rep):
		QThread(), mRep(rep) {}
	/**render the rep in a secondary thread.
	 *
	 * -using ssc::View crashes (Qt in another thread),
	 * -pure vtk gives a popup window, but seems to work ok.
	 * -The sleep way gives the rest of the gui a chance to render before rendering
	 *  the volumetricrep, thus prettying up things a bit. Still the main thread hangs
	 *  4.5 secs when the render is called.
	 */
	virtual void run()
	{
	//	msleep(50);

		vtkRenderWindowPtr window = vtkRenderWindowPtr::New();
		window->BordersOff();
		window->SetSize(1,1); // the price we pay: a 1 pixel dot in the upper left corner.
	//	window->SetSize(5,5);
		vtkRendererPtr renderer = vtkRendererPtr::New();
		window->AddRenderer(renderer);
		renderer->AddVolume(mRep->getVtkVolume());
		window->Render();

	//	ssc::View* view = new ssc::View();
	//	view->addRep(mRep);
	//	view->GetRenderWindow()->GetInteractor()->Disable();
	//	view->GetRenderWindow()->Render();
	//	view->removeReps();
	//	delete view;
	}
	VolumetricRepPtr rep()
	{
		return mRep;
	}
private:
	VolumetricRepPtr mRep;
};


// --------------------------------------------------------
// --------------------------------------------------------

// --------------------------------------------------------
// --------------------------------------------------------

ProgressiveLODVolumetricRep::ProgressiveLODVolumetricRep(const QString& uid, const QString& name) :
	VolumetricBaseRep(uid, name)
{
	mMaxVoxels = 0;
	mClearing = false;
	mAssembly = vtkAssemblyPtr::New();
	mView = NULL;
	resetResampleList();
}

void ProgressiveLODVolumetricRep::resetResampleList()
{
	double r = VolumetricRep::computeResampleFactor(mMaxVoxels, mImage);

	mResampleFactors.clear();
	mResampleFactors.push_back(r);
	mResampleFactors.push_back(r/2.0);
	mResampleFactors.push_back(r/4.0);
	mResampleFactors.push_back(r/8.0);
}


void ProgressiveLODVolumetricRep::setMaxVolumeSize(long maxVoxels)
{
	mMaxVoxels = maxVoxels;
}

ProgressiveLODVolumetricRep::~ProgressiveLODVolumetricRep()
{
	clearThreads();
}

ProgressiveLODVolumetricRepPtr ProgressiveLODVolumetricRep::New(const QString& uid, const QString& name)
{
	ProgressiveLODVolumetricRepPtr retval(new ProgressiveLODVolumetricRep(uid));
	retval->mSelf = retval;
	return retval;
}

void ProgressiveLODVolumetricRep::addRepActorsToViewRenderer(View *view)
{
	view->getRenderer()->AddVolume(mAssembly);
}

void ProgressiveLODVolumetricRep::removeRepActorsFromViewRenderer(View *view)
{
	view->getRenderer()->RemoveVolume(mAssembly);
}

ImagePtr ProgressiveLODVolumetricRep::getImage()
{
	if (!mCurrent)
		return ImagePtr();
	return mCurrent->getImage();
}

vtkVolumePtr ProgressiveLODVolumetricRep::getVtkVolume()
{
	if (!mCurrent)
		return vtkVolumePtr();
	return mCurrent->getVtkVolume();
}

void ProgressiveLODVolumetricRep::setImage(ImagePtr image)
{
	if (image==mImage)
	{
		return;
	}

	if (mImage)
	{
		mAssembly->RemovePart(mCurrent->getVtkVolume());
		mCurrent.reset();
		clearThreads();
	}

	mImage = image;

	if (mImage)
	{
		this->resetResampleList();
		mCurrent = getNextResampleLevel();
		mAssembly->AddPart(mCurrent->getVtkVolume());
		this->startThread(getNextResampleLevel());
		emit internalVolumeChanged();
	}
}

/**Get a volumetricrep with the next in line resample rate.
 *
 */
VolumetricRepPtr ProgressiveLODVolumetricRep::getNextResampleLevel()
{
	if (mResampleFactors.empty())
		return VolumetricRepPtr();
	double factor = mResampleFactors.back();
	QString text = "_" + qstring_cast(factor);
	mResampleFactors.pop_back();

	VolumetricRepPtr next = ssc::VolumetricRep::New(getUid()+text, getName()+text);
	next->setResampleFactor(factor);
	next->setImage(mImage);
	return next;
}

/**Fire one render thread
 *
 */
void ProgressiveLODVolumetricRep::startThread(VolumetricRepPtr rep)
{
	if (!rep)
		return;
	if (mClearing)
		return;
	mThread.reset(new VolumetricRepThreadedRenderer(rep));
	connect(mThread.get(), SIGNAL(finished()), this, SLOT(volumetricThreadFinishedSlot()));
	mThread->start();
}

bool ProgressiveLODVolumetricRep::hasImage(ImagePtr image) const
{
	return (( mCurrent != NULL )&&( mCurrent->getImage() != NULL ));
}

void ProgressiveLODVolumetricRep::clearThreads()
{
	if (mThread)
	{
		mClearing = true;
		disconnect(mThread.get(), SIGNAL(finished()), this, SLOT(volumetricThreadFinishedSlot()));
		mThread->wait(10000); // wait 10s for each thread... should be fast though.
		mClearing = false;
	}
}

/**one rendering finished: replace volume im assembly and start another thread
 * if necessary.
 */
void ProgressiveLODVolumetricRep::volumetricThreadFinishedSlot()
{
	if (mCurrent)
	{
		mAssembly->RemovePart(mCurrent->getVtkVolume());
	}

	mCurrent  = mThread->rep();

	if (mCurrent)
	{
		mAssembly->AddPart(mCurrent->getVtkVolume());
	}

	startThread(getNextResampleLevel());

	emit internalVolumeChanged();
}


} // namespace ssc
