/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#include "cxTrackingDataToVolume.h"

#include <vtkImageData.h>

#include "cxBoundingBox3D.h"
#include "cxDataManager.h"
#include "cxTrackingService.h"
#include "cxRegistrationTransform.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxVolumeHelpers.h"
#include "cxReporter.h"
//#include "cxStateService.h"
//#include "cxPatientData.h"

#include "cxLegacySingletons.h"
#include "cxSpaceProvider.h"

namespace cx
{
TrackingDataToVolume::TrackingDataToVolume()
{}

TrackingDataToVolume::~TrackingDataToVolume()
{}

std::vector<Vector3D> TrackingDataToVolume::extractPoints(TimedTransformMap& map_prMt)
{
  TimedTransformMap::iterator mapIter = map_prMt.begin();
    std::vector<Vector3D> positions_pr;
    while(mapIter != map_prMt.end())
    {
      Vector3D point_t = Vector3D(0,0,0);
      positions_pr.push_back(mapIter->second.coord(point_t));
			++mapIter;
    }
    return positions_pr;
}

DoubleBoundingBox3D TrackingDataToVolume::getBoundingBox(TimedTransformMap& map_prMt)
{
  std::vector<Vector3D> positions_pr = extractPoints(map_prMt);
  return DoubleBoundingBox3D::fromCloud(positions_pr);
}

ImagePtr TrackingDataToVolume::createEmptyImage(DoubleBoundingBox3D bounds_pr, double spacing)
{
  //std::cout << "bounds:" << bounds_pr << std::endl;
  //std::cout << "range:" << bounds_pr.range() << std::endl;
  Eigen::Array3i dim = (ceil(bounds_pr.range() / spacing) + Vector3D(1,1,1)).cast<int>();

  double maxVolumeSize = 10000000;//TODO: Set a good max value or set this as a parameter
  double size = dim[0]*dim[1]*dim[2];
  if(size > maxVolumeSize)
  {
    reportWarning("Tool position volume is going to be to big, making a smaller one.");
    spacing *= pow(size / maxVolumeSize, 1.0/3);
    dim = (ceil(bounds_pr.range() / spacing) + Vector3D(1,1,1)).cast<int>();
  }

  Vector3D spacingVector = Vector3D(1,1,1) * spacing;
  //std::cout << "dim: " << dim << std::endl;
  vtkImageDataPtr data_pr = generateVtkImageData(dim, spacingVector, 0);

  ImagePtr image = dataManager()->createImage(data_pr, "tc%1", "Tool positions %1", "Images");
  dataManager()->loadData(image);
  return image;
}

void TrackingDataToVolume::insertPoints(ImagePtr image_d, std::vector<Vector3D> points_pr, int padding_voxels)
{
  vtkImageDataPtr data_pr = image_d->getBaseVtkImageData();

  //convert points into image space (d) and insert a binary value into the image at the points location in the image
  CoordinateSystem pr = spaceProvider()->getPr();
  CoordinateSystem d = spaceProvider()->getD(image_d);
  Transform3D dMpr = spaceProvider()->get_toMfrom(pr, d);

  std::vector<Vector3D>::iterator it = points_pr.begin();
  for(; it != points_pr.end(); ++it)
  {
    Vector3D point_d = dMpr.coord((*it));
    Vector3D point_voxel = divide_elems(point_d, Vector3D(data_pr->GetSpacing()));
    point_voxel = round(point_voxel);
    this->writeVoxelValuesWithPadding(point_voxel, data_pr, padding_voxels);
  }
}

void TrackingDataToVolume::writeVoxelValuesWithPadding(Vector3D point_voxel, vtkImageDataPtr data_pr, int padding_voxels)
{
  unsigned char* voxel_d;
  unsigned char point_value = 255;
  for(int i=-padding_voxels; i<=padding_voxels; ++i)
  {
    for(int j=-padding_voxels; j<=padding_voxels; ++j)
    {
      for(int k=-padding_voxels; k<=padding_voxels; ++k)
      {
        if(rangeCheck(point_voxel[0]+i, point_voxel[1]+j, point_voxel[2]+k, data_pr->GetExtent()))
        {
          voxel_d = static_cast<unsigned char*>(data_pr->GetScalarPointer(point_voxel[0]+i, point_voxel[1]+j, point_voxel[2]+k));
          (*voxel_d) = point_value;
        }
      }
    }
  }
}

void TrackingDataToVolume::setInput(TimedTransformMap map_prMt, int padding_voxels)
{
  double initialSpacing_mm = 0.2;
  double padding_mm = padding_voxels*initialSpacing_mm;

  DoubleBoundingBox3D bounds_mm = getBoundingBox(map_prMt);
  bounds_mm[0] += -padding_mm;
  bounds_mm[1] += padding_mm;
  bounds_mm[2] += -padding_mm;
  bounds_mm[3] += padding_mm;
  bounds_mm[4] += -padding_mm;
  bounds_mm[5] += padding_mm;
  mImage = createEmptyImage(bounds_mm, initialSpacing_mm);

  Transform3D rMpr = dataManager()->get_rMpr();
  //std::cout << "rMpr\n" << rMpr << std::endl;
  Transform3D rMd = rMpr * createTransformTranslate(bounds_mm.corner(0,0,0)); // TODO + eller - ?????
  //std::cout << "rMd\n" << rMd << std::endl;

  mImage->get_rMd_History()->setRegistration(rMd);

  std::vector<Vector3D> data_pr = this->extractPoints(map_prMt);
  this->insertPoints(mImage, data_pr, padding_voxels);
}

ImagePtr TrackingDataToVolume::getOutput()
{
  return mImage;
}
}//cx

