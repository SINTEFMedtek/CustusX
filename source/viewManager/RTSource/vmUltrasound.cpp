#include "vmUltrasound.h"

#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkImageData.h>
#include <vtkPlaneSource.h>
#include <vtkTransformTextureCoords.h>
#include <vtkTextureMapToPlane.h>
#include <vtkDataSetMapper.h>
#include <vtkTexture.h>
#include <vtkCamera.h>

#include "sscView.h"
#include "Utilities.h"
#include "pdActiveStudy.h"
#include "ViewportDivider.h"
#include "vmUltrasoundPlane.h"
#include "vmViewMachineAPI.h"
#include "sscVtkHelperClasses.h"

//---------------------------------------------------------
namespace vm
{
//---------------------------------------------------------

Ultrasound::Ultrasound(DBusAPIPtr callback)
{
	mActive = false;
	mCallback = callback;
	mData.reset(new UltrasoundData());
}

Ultrasound::~Ultrasound()
{
	deactivate();
}

/**Enter ultrasound mode
 * Controlled by Framework
 */
void Ultrasound::activate()
{
	if (mActive)
	{
		return;
	}

	mCallback->usScannerActivate();
	mActive = true;
}

bool Ultrasound::active() const
{
	return mActive;
}

/**Leave ultrasound mode
 * Controlled by Framework
 */
void Ultrasound::deactivate()
{
	if (!mActive)
	{
		return;
	}

	mCallback->usScannerDeactivate();
	teardown();
	mActive = false;
}

/**start us streaming
 */
void Ultrasound::start(int shmid)
{
	if (!mActive)
	{
		return;
	}
	setup(shmid);
}

/**stop us streaming
 */
void Ultrasound::stop()
{
	teardown();
}

/**set the state to modified, meaning that next call to refresh
 * will update screen anew.
 */
void Ultrasound::setModified()
{

	if (!mActive)
	{
		return;
	}
	mData->setModified();
	mData->startTimer();//start timing the freshness off the image
}

/**Update screen if modified.
 * The reason for this system is that we want to
 * receive the updated message from US Scanner first,
 * then empty the message queue (i.e. dbus), then render.
 */
void Ultrasound::refresh()
{
	if (!mActive)
	{
		return;
	}

	mData->refresh();

}
void Ultrasound::validateData()
{
}

void Ultrasound::setStatus( snw_us::ModuleStatus status, double mi, double ti )
{
	mData->setStatus(status, mi, ti);
}

void Ultrasound::setup(int shmtKey)
{
	mData->connectSource(shmtKey);
}

bool Ultrasound::finished() const
{
	return mData->connected();
}

void Ultrasound::teardown()
{
	mData->disconnectSource();
}

UltrasoundRepPtr Ultrasound::createRep()
{
	//mData->reconnectSource();
	UltrasoundRepPtr retval(new UltrasoundRep(mData, "us_live", "us_live"));
	return retval;
}

ViewControlData Ultrasound::createViewControlData(const ViewControlData& base)
{
	ViewControlData retval = base;
	retval.mLayout = "ULTRASOUND";
	retval.mZoomFactor2D = 1;
	retval.mUseGravity = false;
	retval.mData.clear();

	SeriesData usSeries; // ignore setting layer info
	//usSeries.mUid = "";
	usSeries.M_world_image = Transform3D();
	usSeries.mAligned = true;
	usSeries.mNavigable = true;
	//usSeries.mType = "Live";
	//usSeries.mCenter =

	retval.mData.resize(1);
	retval.mData[0].mName = "Live";
	retval.mData[0].push_back(usSeries);
	retval.mData[0].mViewType = snw::vzRT;

	if (!base.mData.empty())
	{
		CompositionData view2D = base.mData.front(); 
		CompositionData view3D;
		
		if (base.mData.size()>1 && base.mData.back().mViewType==snw::vz3D)
		{
			view3D = base.mData.back();			
		}
		else
		{
			// generate 3D composition based on the 2D data.
			pd::SeriesPtr image = pd::ActiveStudy::instance()->val()->series(view2D.mData.front().uid());
			view3D = view2D;
			view3D.mViewType = snw::vz3D;
			view3D.mData[0].mLayer = createLayerFromSeries(image, snw::vz3D);			
		}

		retval.mData.push_back(view2D);			
		retval.mData.push_back(view3D);			
	}

	return retval;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------



UltrasoundRep::UltrasoundRep(UltrasoundDataPtr data, const std::string& uid, const std::string& name) :
	ssc::RepImpl(uid, name),
	mPlaneActor(vtkActorPtr::New()),
	mPlaneSource(vtkPlaneSourcePtr::New()),
	mUsTexture(vtkTexturePtr::New() )
{
	mData = data;
	connect(mData.get(), SIGNAL(statusChanged()), this, SLOT(statusChangedSlot()));

	mUsTexture->SetInput(mData->getVtkImageData());

	vtkTextureMapToPlanePtr tMapper = vtkTextureMapToPlanePtr::New();
	tMapper->SetInput(mPlaneSource->GetOutput());

	vtkTransformTextureCoordsPtr transform = vtkTransformTextureCoordsPtr::New();
	transform->SetInput(tMapper->GetOutput() );
	transform->SetOrigin( 0, 0, 0);
	transform->SetScale( 1, 1, 0);
	transform->FlipROn();

	vtkDataSetMapperPtr mapper2 = vtkDataSetMapperPtr::New();
	mapper2->SetInput(transform->GetOutput() );
	mapper2->Update();

	mPlaneActor->SetTexture(mUsTexture);
	mPlaneActor->SetMapper(mapper2);

	setup();
	statusChangedSlot();
	//Logger::log("vm.log", "UltrasoundRep::UltrasoundRep()");
}

UltrasoundRep::~UltrasoundRep()
{
//	Logger::log("vm.log", "UltrasoundRep::~UltrasoundRep()");
}

void UltrasoundRep::initializeSize(int imageWidth, int imageHeight)
{
//	Logger::log("vm.log", "UltrasoundRep::initializeSize("+string_cast(imageWidth)+","+string_cast(imageHeight)+")");
	if (imageWidth==0 || imageHeight==0)
	{
		return;
	}
	mPlaneSource->SetPoint2( 0.0, imageHeight - 1, 0.0 );
	mPlaneSource->SetPoint1(imageWidth - 1, 0.0, 0.0 );
}

void UltrasoundRep::statusChangedSlot()
{
	initializeSize(mData->width(), mData->height());
	mPlaneActor->SetVisibility(mData->valid());
	formatMechTermIndex();
	mStatusText->updateText(mData->statusString() );
	setCamera();

	Logger::log("vm.log", "US statusChanged: " + string_cast(mData->width()) + ", " + string_cast(mData->height())
			+ ", connected: " + string_cast(mData->connected()) );

}

/**We need this here, even if it belongs in singlelayout.
 * Reason: must call setcamera after last change of size of plane actor.
 * TODO fix it.
 */
void UltrasoundRep::setCamera()
{
	if (!mRenderer)
		return;
	vtkCamera* camera = mRenderer->GetActiveCamera();
	camera->ParallelProjectionOn();
	mRenderer->ResetCamera();

	double scale = camera->GetParallelScale();

	//SW_LOG("%p, %p, %f", mRenderer.GetPointer(), this, scale);

	//SW_LOG("Scale %f ", scale );
	camera->SetParallelScale(scale*0.6);
}


void UltrasoundRep::addRepActorsToViewRenderer(ssc::View* view)
{


	mRenderer = view->getRenderer();

	view->getRenderer()->AddActor(mPlaneActor);
	view->getRenderer()->AddActor(mInfoText->getActor());
	view->getRenderer()->AddActor(mStatusText->getActor());
	//setCamera();
}

void UltrasoundRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
	mRenderer = vtkRendererPtr();
	view->getRenderer()->RemoveActor(mPlaneActor);
	view->getRenderer()->RemoveActor(mInfoText->getActor());
	view->getRenderer()->RemoveActor(mStatusText->getActor());
}

void UltrasoundRep::setup()
{
	mInfoText.reset(new ssc::TextDisplay("", Vector3D(1.0, 0.8, 0.0), 16));
	mInfoText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	mInfoText->setCentered();
	mInfoText->setPosition(0.5, 0.05);

	mStatusText.reset(new ssc::TextDisplay("", Vector3D(1.0, 0.8, 0.0), 20));
	mStatusText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	mStatusText->setCentered();
	mStatusText->setPosition(0.5, 0.5);
	mStatusText->updateText(mData->statusString());

	//setCamera();
}

void UltrasoundRep::formatMechTermIndex()
{
	char buffer[100];
	snprintf(buffer, sizeof(buffer), "MI %3.1f TI %3.1f", mData->mechanicalIndex(), mData->thermalIndex() );
	mInfoText->updateText(std::string(buffer));
}


//---------------------------------------------------------
} // namespace vm
//---------------------------------------------------------

