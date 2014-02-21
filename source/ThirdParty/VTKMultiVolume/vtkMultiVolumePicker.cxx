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

typedef vtkSmartPointer<class vtkGPUVolumeRayCastMapper> vtkGPUVolumeRayCastMapperPtr;
typedef vtkSmartPointer<class vtkVolumeTextureMapper3D> vtkVolumeTextureMapper3DPtr;
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

//			vtkGPUVolumeRayCastMapperPtr singleVolumeMapper = vtkGPUVolumeRayCastMapperPtr::New();
            vtkVolumeTextureMapper3DPtr singleVolumeMapper = vtkVolumeTextureMapper3DPtr::New();
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

}


