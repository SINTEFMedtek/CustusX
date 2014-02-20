/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVolumePicker.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMultiVolumePicker.h"
#include "vtkVolumePicker.h"
#include "vtkObjectFactory.h"

#include "vtkBox.h"
#include "vtkImageData.h"
#include "vtkVolume.h"
#include "vtkVolumeMapper.h"
#include "vtkOpenGLGPUMultiVolumeRayCastMapper.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkTransform.h"
#include "vtkImageChangeInformation.h"

typedef vtkSmartPointer<class vtkGPUVolumeRayCastMapper> vtkGPUVolumeRayCastMapperPtr;
typedef vtkSmartPointer<class vtkVolume> vtkVolumePtr;
typedef vtkSmartPointer<class vtkTransform> vtkTransformPtr;
typedef vtkSmartPointer<class vtkMatrix4x4> vtkMatrix4x4Ptr;
typedef vtkSmartPointer<vtkImageChangeInformation> vtkImageChangeInformationPtr;

vtkStandardNewMacro(vtkMultiVolumePicker);

//----------------------------------------------------------------------------
vtkMultiVolumePicker::vtkMultiVolumePicker()
{
}

//----------------------------------------------------------------------------
vtkMultiVolumePicker::~vtkMultiVolumePicker()
{
}

//----------------------------------------------------------------------------
void vtkMultiVolumePicker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

//  os << indent << "PickCroppingPlanes: "
//     << (this->PickCroppingPlanes ? "On" : "Off") << "\n";

//  os << indent << "CroppingPlaneId: " << this->CroppingPlaneId << "\n";
}

//----------------------------------------------------------------------------
void vtkMultiVolumePicker::ResetPickInfo()
{
  this->Superclass::ResetPickInfo();
  
//  this->CroppingPlaneId = -1;
}

//----------------------------------------------------------------------------
// Intersect a vtkVolume with a line by ray casting.  Compared to the
// same method in the superclass, this method will look for cropping planes.

double vtkMultiVolumePicker::IntersectVolumeWithLine(const double p1[3],
                                                const double p2[3],
                                                double t1, double t2,
                                                vtkProp3D *prop, 
                                                vtkAbstractVolumeMapper *mapper)
{
	vtkOpenGLGPUMultiVolumeRayCastMapper* multivolumeMapper = dynamic_cast<vtkOpenGLGPUMultiVolumeRayCastMapper*>(mapper);
	if(multivolumeMapper)
	{
//		std::cout << "multivolumeMapper NUMBER_OF_ADDITIONAL_VOLUMES: " << multivolumeMapper->NUMBER_OF_ADDITIONAL_VOLUMES << std::endl;
		double retval = VTK_DOUBLE_MAX;

		for(int i = 0; i < multivolumeMapper->NUMBER_OF_ADDITIONAL_VOLUMES; ++i)
		{
			double tempRetval;

			vtkVolumePtr volume = vtkVolumePtr::New();
			vtkImageData* image = multivolumeMapper->GetInput(i+1);
			vtkVolumeProperty* property = multivolumeMapper->GetAdditionalProperty(i);

			vtkMatrix4x4Ptr rMd0 = prop->GetUserMatrix();
			if (!rMd0)
			{
//				std::cout << "No user matrix in prop" << std::endl;
				rMd0 = vtkMatrix4x4Ptr::New();
				rMd0->Identity();
			}
			vtkTransform* transform = multivolumeMapper->GetAdditionalInputUserTransform(i); //transform is d0Mdi
			transform->PostMultiply();
			transform->Concatenate(rMd0);//rMd0 * d0Mdi -> transform is now rMdi

			double trans_pos[3];
			transform->GetPosition(trans_pos);

			vtkImageChangeInformationPtr info = vtkImageChangeInformationPtr::New();
			info->SetInput(image);
			info->SetOutputOrigin(trans_pos);
			vtkImageData* tempImage = info->GetOutput();

			vtkGPUVolumeRayCastMapperPtr singleVolumeMapper = vtkGPUVolumeRayCastMapperPtr::New();
			singleVolumeMapper->SetBlendModeToComposite();
			singleVolumeMapper->SetInput(tempImage);
			singleVolumeMapper->Update();

			volume->SetMapper(singleVolumeMapper);
			volume->SetProperty(property);
//			volume->SetUserTransform(transform); //Transform is not used by IntersectVolumeWithLine(), only vtkImageData::Origin()
//			volume->Update();

			tempRetval = this->Superclass::IntersectVolumeWithLine(p1, p2, t1, t2, volume, singleVolumeMapper);
//			std::cout << "temp retval: " << tempRetval << std::endl;
			retval = fmin(retval, tempRetval);
		}

		return retval;
	} else
		return this->Superclass::IntersectVolumeWithLine(p1, p2, t1, t2, prop, mapper);
/*
	double tMin = VTK_DOUBLE_MAX;

  vtkImageData *data = vtkImageData::SafeDownCast(mapper->GetDataSetInput());
  vtkVolumeMapper *vmapper = vtkVolumeMapper::SafeDownCast(mapper);
  
  if (data == 0)
    {
    // This picker only works with image inputs
    return VTK_DOUBLE_MAX;
    }

  // Convert ray to structured coordinates
  double spacing[3], origin[3];
  int extent[6];
  data->GetSpacing(spacing);
  data->GetOrigin(origin);
  data->GetExtent(extent);

  double x1[3], x2[3];
  for (int i = 0; i < 3; i++)
    {
    x1[i] = (p1[i] - origin[i])/spacing[i];
    x2[i] = (p2[i] - origin[i])/spacing[i];
    }

  // These are set to the plane that the ray enters through
  int planeId = -1;
  int extentPlaneId = -1;

  // There might be multiple regions, depending on cropping flags
  int numSegments = 1;
  double t1List[16], t2List[16], s1List[16];
  int planeIdList[16];
  t1List[0] = t1;
  t2List[0] = t2;
  // s1 is the cropping plane intersection, initialize to large value
  double s1 = s1List[0] = VTK_DOUBLE_MAX;
  planeIdList[0] = -1;
 
  // Find the cropping bounds in structured coordinates
  double bounds[6];
  for (int j = 0; j < 6; j++)
    {
    bounds[j] = extent[j];
    }

  if (vmapper && vmapper->GetCropping())
    {
    vmapper->GetCroppingRegionPlanes(bounds);
    for (int j = 0; j < 3; j++)
      {
      double b1 = (bounds[2*j] - origin[j])/spacing[j]; 
      double b2 = (bounds[2*j+1] - origin[j])/spacing[j]; 
      bounds[2*j] = (b1 < b2 ? b1 : b2);
      bounds[2*j+1] = (b1 < b2 ? b2 : b1);
      if (bounds[2*j] < extent[2*j]) { bounds[2*j] = extent[2*j]; }
      if (bounds[2*j+1] > extent[2*j+1]) { bounds[2*j+1] = extent[2*j+1]; }
      if (bounds[2*j] > bounds[2*j+1])
        {
        return VTK_DOUBLE_MAX;
        }
      }

    // Get all of the line segments that intersect the visible blocks
    int flags = vmapper->GetCroppingRegionFlags();
    if (!this->ClipLineWithCroppingRegion(bounds, extent, flags, x1, x2,
                                          t1, t2, extentPlaneId, numSegments,
                                          t1List, t2List, s1List, planeIdList))
      {
      return VTK_DOUBLE_MAX;
      }
    }
  else
    {
    // If no cropping, then use volume bounds
    double s2;
    if (!this->ClipLineWithExtent(extent, x1, x2, s1, s2, extentPlaneId))
      {
      return VTK_DOUBLE_MAX;
      }
    s1List[0] = s1;
    t1List[0] = ( (s1 > t1) ? s1 : t1 );
    t2List[0] = ( (s2 < t2) ? s2 : t2 );
    }

  if (this->PickCroppingPlanes && vmapper && vmapper->GetCropping())
    {
    // Only require information about the first intersection
    s1 = s1List[0];
    if (s1 > t1)
      {
      planeId = planeIdList[0];
      }

    // Set data values at the intersected cropping or clipping plane
    if ((tMin = t1List[0]) < this->GlobalTMin)
      {
      this->ResetPickInfo();
      this->DataSet = data;
      this->Mapper = vmapper;

      double x[3];
      for (int j = 0; j < 3; j++)
        {
        x[j] = x1[j]*(1.0 - tMin) + x2[j]*tMin;
        if (planeId >= 0 && j == planeId/2)
          {
          x[j] = bounds[planeId];
          }
        else if (planeId < 0 && extentPlaneId >= 0 && j == extentPlaneId/2)
          {
          x[j] = extent[extentPlaneId];
          }
        this->MapperPosition[j] = x[j]*spacing[j] + origin[j];
        }

      this->SetImageDataPickInfo(x, extent);
      }
    }
  else
    {
    // Go through the segments in order, until a hit occurs
    for (int segment = 0; segment < numSegments; segment++)
      {
      if ((tMin = this->Superclass::IntersectVolumeWithLine(
           p1, p2, t1List[segment], t2List[segment], prop, mapper))
           < VTK_DOUBLE_MAX)
        {
        s1 = s1List[segment];
        // Keep the first planeId that was set at the first intersection
        // that occurred after t1
        if (planeId < 0 && s1 > t1)
          {
          planeId = planeIdList[segment];
          }
        break;
        }
      }
    }

  if (tMin < this->GlobalTMin)
    {
    this->CroppingPlaneId = planeId;
    // If t1 is at a cropping or extent plane, use the plane normal
    if (planeId < 0)
      {
      planeId = extentPlaneId;
      }
    if (planeId >= 0 && tMin == s1)
      {
      this->MapperNormal[0] = 0.0;
      this->MapperNormal[1] = 0.0;
      this->MapperNormal[2] = 0.0;
      this->MapperNormal[planeId/2] = 2.0*(planeId%2) - 1.0;
      if (spacing[planeId/2] < 0)
        {
        this->MapperNormal[planeId/2] = - this->MapperNormal[planeId/2];
        }
      }
    }

  return tMin;
  */
}


