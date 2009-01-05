#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include "sscSlicerRepSW.h"
#include "sscView.h"

#include "sscBoundingBox3D.h"

namespace ssc
{

SliceRepSW::SliceRepSW(const std::string& uid) :
	RepImpl(uid)
{

	mReslicer = vtkImageReslicePtr::New();
	mMatrixAxes = vtkMatrix4x4Ptr::New();
	mImageActor = vtkImageActorPtr::New();
	mWindowLevel = vtkImageMapToWindowLevelColorsPtr::New();
	mLookupTable = vtkWindowLevelLookupTablePtr::New();

	//*create default lut
	setLookupTable();
}
SliceRepSW::~SliceRepSW()
{

}
SliceRepSWPtr SliceRepSW::New(const std::string& uid)
{
	SliceRepSWPtr retval(new SliceRepSW(uid));
	retval->mSelf = retval;
	return retval;
}

void SliceRepSW::setImage(ImagePtr image)
{
	std::cout<<"setImage"<<std::endl;
	mImage = image;
	mImage->connectRep(mSelf);
}

void SliceRepSW::setTool(ToolPtr tool)
{
	std::cout<<"setTool"<<std::endl;
	mTool = tool;
	
	connect( mTool.get(), SIGNAL( toolTransformAndTimestamp(Transform3D ,double) ), this, SLOT( updateToolTransform(Transform3D,double) ));

}
void SliceRepSW::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddActor(mImageActor);
}
void SliceRepSW::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveActor(mImageActor);
}
bool SliceRepSW::hasImage(ImagePtr image) const
{
	return (mImage != NULL);
}
void SliceRepSW::setOrientation(SliceComputer::PLANE_TYPE planeType)
{
	mPlaneType = planeType;
	compute();
}
void SliceRepSW::setFollowType(SliceComputer::FOLLOW_TYPE followType)
{
	mFollowType = followType;
}
void SliceRepSW::compute()
{
	if ( mImage == NULL)
	{
		return;
	}
	DoubleBoundingBox3D bounds( mImage->getBaseVtkImageData()->GetBounds() );
	Vector3D center = bounds.center();
	
	std::cout << "Center :\n"<< center<<std::endl;
	
	mCutplane.setOrientationType(ssc::SliceComputer::otORTHOGONAL);
	mCutplane.setFollowType(mFollowType);
	mCutplane.setPlaneType(mPlaneType);
	mCutplane.setFixedCenter(center);
	SlicePlane plane = mCutplane.getPlane();

	this->setMatrixData(plane.i, plane.j, plane.c);
	this->doSliceing();
}
void SliceRepSW::doSliceing()
{
	mReslicer->SetInput(mImage->getBaseVtkImageData() );
	mReslicer->SetInterpolationModeToLinear();
	mReslicer->GetOutput()->UpdateInformation();
	mReslicer->SetOutputDimensionality( 2);
	mReslicer->SetResliceAxes(mMatrixAxes) ;
	mReslicer->SetAutoCropOutput(false);

	mWindowLevel->SetInputConnection(mReslicer->GetOutputPort() );
	mImageActor->SetInput( mWindowLevel->GetOutput() );
}

void SliceRepSW::setMatrixData(const Vector3D& ivec, const Vector3D& jvec, const Vector3D& center)
{
	Vector3D kvec = cross(ivec, jvec);
	//set all column vectors
	mMatrixAxes->Identity();
	for (unsigned i = 0; i < 3; ++i)
	{
		mMatrixAxes->SetElement(i, 0, ivec[i]);
		mMatrixAxes->SetElement(i, 1, jvec[i]);
		mMatrixAxes->SetElement(i, 2, kvec[i]);
		mMatrixAxes->SetElement(i, 3, center[i]);
	}
}

//**SLOTS***//
void SliceRepSW::updateToolTransform(Transform3D matrix, double timestamp)
{
	mCutplane.setToolPosition( matrix );
	SlicePlane plane = mCutplane.getPlane();
	std::cout << "plane: "<< this->getUid()<<"\n" << mCutplane.getPlane() << std::endl;
	this->setMatrixData( plane.i, plane.j, plane.c );
} 

void SliceRepSW::setLookupTable()
{
	mLookupTable->SetTableRange(0, 1);
	mLookupTable->SetSaturationRange(0, 0);
	mLookupTable->SetHueRange(0, 0);
	mLookupTable->SetValueRange(0, 1);
	mLookupTable->Build();
	std::cout<<"add Lookup table"<<std::endl;
	mWindowLevel->SetLookupTable(mLookupTable);
	mWindowLevel->SetWindow( 255.0);
	mWindowLevel->SetLevel( 127.5);
}

}// namespace ssc
