
#include "cxTrackingDataToVolume.h"

#include <vtkImageData.h>

#include "sscBoundingBox3D.h"
#include "sscDataManager.h"
#include "utils/sscReconstructHelper.h"
#include "sscRegistrationTransform.h"
#include "cxStateMachineManager.h"
#include "cxPatientData.h"

namespace cx
{
TrackingDataToVolume::TrackingDataToVolume() :
    mSpacing(0.2)
{
}

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

void TrackingDataToVolume::insertPoints(ssc::ImagePtr image_pr, std::vector<ssc::Vector3D> points_pr)
{
//TODO
  vtkImageDataPtr data_pr = image_pr->getBaseVtkImageData();
  //unsigned char* dataPtr = data_pr->GetScalarPointer();
}

void TrackingDataToVolume::setInput(ssc::TimedTransformMap map_prMt)
{
  ssc::DoubleBoundingBox3D extent_pr = getBoundingBox(map_prMt);
  mImage = createEmptyImage(extent_pr, mSpacing);

  std::vector<ssc::Vector3D> data_pr = this->extractPoints(map_prMt);
  this->insertPoints(mImage, data_pr);

  mImage->get_rMd_History()->setRegistration(ssc::Transform3D());
}

ssc::ImagePtr TrackingDataToVolume::getOutput()
{
  return mImage;
}
}//cx

