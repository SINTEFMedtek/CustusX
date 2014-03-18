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
#include "vtkMath.h"

typedef vtkSmartPointer<class vtkVolume> vtkVolumePtr;
typedef vtkSmartPointer<vtkImageChangeInformation> vtkImageChangeInformationPtr;

vtkStandardNewMacro(vtkMultiVolumePicker);

//----------------------------------------------------------------------------
vtkMultiVolumePicker::vtkMultiVolumePicker()
{
	usedSingleVolumePicker = false;
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
		usedSingleVolumePicker = false;
		double retval = VTK_DOUBLE_MAX;

		for(int i = 0; i < multivolumeMapper->NUMBER_OF_ADDITIONAL_VOLUMES; ++i)
		{
			vtkMatrix4x4Ptr rMd0 = prop->GetUserMatrix();
			vtkTransformPtr d0Mdi = multivolumeMapper->GetAdditionalInputUserTransform(i); //transform is d0Mdi
			vtkTransformPtr rMdi = this->calculate_rMdi(rMd0, d0Mdi);
			double newOrigin[3];
			rMdi->GetPosition(newOrigin);
			this->calculateNewOrigin(newOrigin, rMd0);

			vtkImageDataPtr image = multivolumeMapper->GetInput(i+1);
			vtkAbstractVolumeMapperPtr singleVolumeMapper = this->generateSingleVolumeMapper(image);
			vtkVolumeProperty* property = multivolumeMapper->GetAdditionalProperty(i);

			vtkVolumePtr volume = vtkVolumePtr::New();
			volume->SetMapper(singleVolumeMapper);
			volume->SetProperty(property);
//			volume->SetUserTransform(rMdi); //Transform is not used by IntersectVolumeWithLine(), only vtkImageData::Origin()

			vtkTransformPtr diMd0 = vtkTransformPtr::New();
			diMd0->DeepCopy(d0Mdi);

			double p1_t[3];
			double p2_t[3];
			diMd0->Inverse();
			diMd0->PreMultiply();
			diMd0->TransformPoint(p1, p1_t);
			diMd0->TransformPoint(p2, p2_t);

//			diMd0->Print(std::cout);

//			std::cout << "p1: " << p1[0] << " " << p1[1] << " " << p1[2] << std::endl;
//			std::cout << "p2: " << p2[0] << " " << p2[1] << " " << p2[2] << std::endl;
//			std::cout << "p1_t: " << p1_t[0] << " " << p1_t[1] << " " << p1_t[2] << std::endl;
//			std::cout << "p2_t: " << p2_t[0] << " " << p2_t[1] << " " << p2_t[2] << std::endl;

			//dist should be equal to dist_t since this a percentage of the total distance between p1 and p2

			double dist_t = this->Superclass::IntersectVolumeWithLine(p1_t, p2_t, t1, t2, volume, singleVolumeMapper);
			double p3[3] = {0, 0, 0};
			if(dist_t < VTK_DOUBLE_MAX)
			{
				//    dist(%)
				// p1 ----- p3 ---- p2
				//
				//     diMd0
				//
				//     dist_t(%)
				// p1_t ----- p3_t ---- p2_t

				//              Normalized vector
				//			p3 = (p2 - p1) / abs(p2 - p1)  * dist + p2;

//				double p1_to_p3_t[3];
//				vtkMath::Subtract(p2_t, p1_t, p1_to_p3_t);//Get vector from p1_t to p2_t
//				std::cout << "p1_to_p3_t before norm: " << p1_to_p3_t[0] << " " << p1_to_p3_t[1] << " " << p1_to_p3_t[2] << std::endl;
//				vtkMath::Normalize(p1_to_p3_t);//Normalize vector
//				std::cout << "p1_to_p3_t after norm: " << p1_to_p3_t[0] << " " << p1_to_p3_t[1] << " " << p1_to_p3_t[2] << std::endl;

//				double dist_p1_p2_t = sqrt(vtkMath::Distance2BetweenPoints(p1_t, p2_t));
//				std::cout << "dist p1_t to p2_t: " << dist_p1_p2_t << std::endl;
//				vtkMath::MultiplyScalar(p1_to_p3_t, dist_p1_p2_t*dist_t);//Multiply normalized vector by distance from p1_t to p3_t get vector from p1_t to p3_t
//				std::cout << "p1_to_p3_t with dist: " << p1_to_p3_t[0] << " " << p1_to_p3_t[1] << " " << p1_to_p3_t[2] << std::endl;
//				double p3_t[3];
//				vtkMath::Add(p1_to_p3_t, p1_t, p3_t);//Find p3_t

//				d0Mdi->TransformPoint(p3_t, p3);
//				std::cout << "p3_t: " << p3_t[0] << " " << p3_t[1] << " " << p3_t[2] << std::endl;
//				std::cout << "p3: " << p3[0] << " " << p3[1] << " " << p3[2] << std::endl;

//				dist = sqrt(vtkMath::Distance2BetweenPoints(p1, p3)) / sqrt(vtkMath::Distance2BetweenPoints(p1, p2));


				double p1_to_p3[3];
				vtkMath::Subtract(p2, p1, p1_to_p3); //Get vector from p1 to p2
				vtkMath::Normalize(p1_to_p3); //Normalize vector
				double dist_p1_p2 = sqrt(vtkMath::Distance2BetweenPoints(p1, p2));
				vtkMath::MultiplyScalar(p1_to_p3, dist_p1_p2*dist_t);//Multiply nomralized vector with dist from p1 tp p3 to get vector from p1 to p3
				vtkMath::Add(p1, p1_to_p3, p3);//Find p3
//				std::cout << "alternative p3: " << p3[0] << " " << p3[1] << " " << p3[2] << std::endl;

//				std::cout << "dist_t: " << dist_t << std::endl;
			}


//			std::cout << "dist: " << dist << std::endl;

			retval = fmin(retval, dist_t);
			if(similar(retval, dist_t))
				this->storeFoundImage(image, singleVolumeMapper, p3);
		}

		return retval;
	} else
	{
		usedSingleVolumePicker = true;
		return this->Superclass::IntersectVolumeWithLine(p1, p2, t1, t2, prop, mapper);
	}

}

vtkTransformPtr vtkMultiVolumePicker::calculate_rMdi(vtkMatrix4x4Ptr rMd0, vtkTransformPtr d0Mdi)
{
	if (!rMd0)
	{
		std::cout << "vtkMultiVolumePicker::calculate_rMdi(): No rMd0, setting it to identity" << std::endl;
		rMd0 = vtkMatrix4x4Ptr::New();
		rMd0->Identity();
	}

	vtkTransformPtr retval = vtkTransformPtr::New();
	retval->DeepCopy(d0Mdi);
	retval->PostMultiply();
	retval->Concatenate(rMd0);
	return retval;
}

void vtkMultiVolumePicker::calculateNewOrigin(double* newOrigin, vtkMatrix4x4Ptr rMd0)
{
	double oldOrigin[3];
	oldOrigin[0] = rMd0->GetElement(0, 3);
	oldOrigin[1] = rMd0->GetElement(1, 3);
	oldOrigin[2] = rMd0->GetElement(2, 3);

	newOrigin[0] = newOrigin[0] - oldOrigin[0];
	newOrigin[1] = newOrigin[1] - oldOrigin[1];
	newOrigin[2] = newOrigin[2] - oldOrigin[2];
}

vtkGPUVolumeRayCastMapperPtr vtkMultiVolumePicker::generateSingleVolumeMapper(vtkImageDataPtr tempImage)
{
	vtkGPUVolumeRayCastMapperPtr singleVolumeMapper = vtkGPUVolumeRayCastMapperPtr::New();
//	vtkVolumeTextureMapper3DPtr singleVolumeMapper = vtkVolumeTextureMapper3DPtr::New();
	singleVolumeMapper->SetBlendModeToComposite();
	singleVolumeMapper->SetInput(tempImage);
	singleVolumeMapper->Update();
	return singleVolumeMapper;
}

bool vtkMultiVolumePicker::similar(double a, double b, double tol)
{
	return fabs(b - a) < tol;
}

void vtkMultiVolumePicker::storeFoundImage(vtkDataSet* image, vtkAbstractVolumeMapper* mapper, double* pickPosition)
{
	this->DataSet = image;
	this->Mapper = mapper;
	this->MultivolumePickPosition[0] = pickPosition[0];
	this->MultivolumePickPosition[1] = pickPosition[1];
	this->MultivolumePickPosition[2] = pickPosition[2];
}

double* vtkMultiVolumePicker::GetPickPosition()
{
	if (usedSingleVolumePicker)
		return this->Superclass::GetPickPosition();

	//p1, p2 are in mapper coord -> see vtkPicker.cxx line 358 and 386
	//Use the missing transform (this->Transform) to get world coordinates

//	std::cout << "PickPosition: " << this->PickPosition[0] << " " << this->PickPosition[1] << " " << this->PickPosition[2] << std::endl;
//	std::cout << "MultivolumePickPosition: " << this->MultivolumePickPosition[0] << " " << this->MultivolumePickPosition[1] << " " << this->MultivolumePickPosition[2] << std::endl;
	this->Transform->TransformPoint(this->MultivolumePickPosition,this->MultivolumePickPosition);
//	std::cout << "New MultivolumePickPosition: " << this->MultivolumePickPosition[0] << " " << this->MultivolumePickPosition[1] << " " << this->MultivolumePickPosition[2] << std::endl;
	return this->MultivolumePickPosition;
}
