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
#include "vtkVolumeTextureMapper3D.h"
#include "vtkTransform.h"
#include "vtkImageChangeInformation.h"

typedef vtkSmartPointer<class vtkVolume> vtkVolumePtr;
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
		double retval = VTK_DOUBLE_MAX;

		for(int i = 0; i < multivolumeMapper->NUMBER_OF_ADDITIONAL_VOLUMES; ++i)
		{
			vtkMatrix4x4Ptr rMd0 = prop->GetUserMatrix();
			vtkTransformPtr d0Mdi = multivolumeMapper->GetAdditionalInputUserTransform(i); //transform is d0Mdi
			vtkTransformPtr rMdi = this->calculate_rMdi(rMd0, d0Mdi);

			double newOrigin[3];
			rMdi->GetPosition(newOrigin);
			vtkImageDataPtr image = multivolumeMapper->GetInput(i+1);
			vtkImageDataPtr tempImage = this->generateImageCopyAndMoveOrigin(image, newOrigin);

			vtkAbstractVolumeMapperPtr singleVolumeMapper = this->generateSingleVolumeMapper(tempImage);
			vtkVolumeProperty* property = multivolumeMapper->GetAdditionalProperty(i);

			vtkVolumePtr volume = vtkVolumePtr::New();
			volume->SetMapper(singleVolumeMapper);
			volume->SetProperty(property);

			double tempRetval = this->Superclass::IntersectVolumeWithLine(p1, p2, t1, t2, volume, singleVolumeMapper);
			retval = fmin(retval, tempRetval);
		}

		return retval;
	} else
		return this->Superclass::IntersectVolumeWithLine(p1, p2, t1, t2, prop, mapper);

}

vtkTransformPtr vtkMultiVolumePicker::calculate_rMdi(vtkMatrix4x4Ptr rMd0, vtkTransformPtr d0Mdi)
{
	if (!rMd0)
	{
		rMd0 = vtkMatrix4x4Ptr::New();
		rMd0->Identity();
	}

	vtkTransformPtr retval = d0Mdi;
	retval->PostMultiply();
	retval->Concatenate(rMd0);
	return retval;
}

vtkImageDataPtr vtkMultiVolumePicker::generateImageCopyAndMoveOrigin(vtkImageDataPtr image, double* newOrigin)
{
	vtkImageChangeInformationPtr info = vtkImageChangeInformationPtr::New();
	info->SetInput(image);
	info->SetOutputOrigin(newOrigin);
	vtkImageDataPtr retval = info->GetOutput();
	return retval;
}

vtkVolumeTextureMapper3DPtr vtkMultiVolumePicker::generateSingleVolumeMapper(vtkImageDataPtr tempImage)
{
	//vtkGPUVolumeRayCastMapperPtr singleVolumeMapper = vtkGPUVolumeRayCastMapperPtr::New();
	vtkVolumeTextureMapper3DPtr singleVolumeMapper = vtkVolumeTextureMapper3DPtr::New();
	singleVolumeMapper->SetBlendModeToComposite();
	singleVolumeMapper->SetInput(tempImage);
	singleVolumeMapper->Update();
	return singleVolumeMapper;
}


