#include "cxPatientLandmarkRep.h"

#include <vtkFollower.h>
#include "sscCoordinateSystemHelpers.h"
#include "cxToolManager.h"

namespace cx
{

PatientLandmarkRepPtr PatientLandmarkRep::New(const QString& uid, const QString& name)
{
  PatientLandmarkRepPtr retval(new PatientLandmarkRep(uid, name));
  retval->mSelf = retval;
  return retval;
}

PatientLandmarkRep::PatientLandmarkRep(const QString& uid, const QString& name) :
  LandmarkRep(uid, name)
{
//  mType = "cxPatientLandmarkRep";
  this->setSecondaryColor(ssc::Vector3D(0,0.6,0.8));

  mImage = ImageLandmarksSourcePtr(new ImageLandmarksSource());
  this->setPrimarySource(mImage);
  this->setSecondarySource(PatientLandmarksSourcePtr(new PatientLandmarksSource()));
}

PatientLandmarkRep::~PatientLandmarkRep()
{}

//QString PatientLandmarkRep::getType() const
//{
//  return mType;
//}

void PatientLandmarkRep::setImage(ssc::ImagePtr image)
{
	mImage->setImage(image);
}

}//namespace cx
