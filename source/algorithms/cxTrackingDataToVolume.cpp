
#include "cxTrackingDataToVolume.h"

#include <vtkImageData.h>

#include "sscBoundingBox3D.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "utils/sscReconstructHelper.h"
#include "sscRegistrationTransform.h"
#include "sscCoordinateSystemHelpers.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{
TrackingDataToVolume::TrackingDataToVolume() :
    mSpacing(0.2)
{}

TrackingDataToVolume::~TrackingDataToVolume()
{}

std::vector<ssc::Vector3D> TrackingDataToVolume::extractPoints(ssc::TimedTransformMap& map_prMt)
{
  ssc::TimedTransformMap::iterator mapIter = map_prMt.begin();
    std::vector<ssc::Vector3D> positions_pr;
    while(mapIter != map_prMt.end())
    {
      ssc::Vector3D point_t = ssc::Vector3D(0,0,0);
      positions_pr.push_back(mapIter->second.coord(point_t));
      mapIter++;
    }
    return positions_pr;
}

ssc::DoubleBoundingBox3D TrackingDataToVolume::getBoundingBox(ssc::TimedTransformMap& map_prMt)
{
  std::vector<ssc::Vector3D> positions_pr = extractPoints(map_prMt);
  return ssc::DoubleBoundingBox3D::fromCloud(positions_pr);
}

ssc::ImagePtr TrackingDataToVolume::createEmptyImage(ssc::DoubleBoundingBox3D extent_pr, int spacing)
{
  ssc::Vector3D dim = extent_pr.range() / spacing;
  ssc::Vector3D spacingVector = ssc::Vector3D(1,1,1) * spacing;
  vtkImageDataPtr data_pr = ssc::generateVtkImageData(dim, spacingVector, 0);

  QString filePath;
  filePath = stateManager()->getPatientData()->getActivePatientFolder() + "/Images/";
  ssc::ImagePtr image = ssc::dataManager()->createImage(data_pr, "tc%1", "Tracked centerline #%1", filePath);
  return image;
}

void TrackingDataToVolume::insertPoints(ssc::ImagePtr image_d, std::vector<ssc::Vector3D> points_pr)
{
  //TODO get someone to "double-check these numbers..." :P

  vtkImageDataPtr data_pr = image_d->getBaseVtkImageData();

  //convert points into image space (d) and insert a binary value into the image at the points location in the image
  ssc::CoordinateSystem pr = ssc::CoordinateSystemHelpers::getPr();
  ssc::CoordinateSystem d = ssc::CoordinateSystemHelpers::getD(image_d);
  ssc::Transform3D dMpr = ssc::CoordinateSystemHelpers::get_toMfrom(pr, d);

  std::vector<ssc::Vector3D>::iterator it = points_pr.begin();
  unsigned char point_value = 1; //or 255???
  for(; it != points_pr.end(); ++it)
  {
    ssc::Vector3D point_d = dMpr.coord((*it));
    unsigned char* voxel_d = static_cast<unsigned char*>(data_pr->GetScalarPointer(point_d[0], point_d[1], point_d[2]));
    (*voxel_d) = point_value;
  }
}

void TrackingDataToVolume::setInput(ssc::TimedTransformMap map_prMt)
{
  ssc::DoubleBoundingBox3D extent = getBoundingBox(map_prMt);
  mImage = createEmptyImage(extent, mSpacing);
  //TODO Should it be identity or rMpr.inv()???
  mImage->get_rMd_History()->setRegistration(ssc::toolManager()->get_rMpr()->inv());

  std::vector<ssc::Vector3D> data_pr = this->extractPoints(map_prMt);
  this->insertPoints(mImage, data_pr);
}

ssc::ImagePtr TrackingDataToVolume::getOutput()
{
  return mImage;
}
}//cx

