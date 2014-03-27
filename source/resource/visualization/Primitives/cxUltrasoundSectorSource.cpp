// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "cxUltrasoundSectorSource.h"

#include "vtkObjectFactory.h"
#include <vtkPointData.h>
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"


vtkStandardNewMacro(UltrasoundSectorSource);

UltrasoundSectorSource::UltrasoundSectorSource()
{
	this->SetNumberOfInputPorts(0);
}

void UltrasoundSectorSource::setProbeSector(vtkPolyDataPtr sector)
{
	mSector = sector;
	this->Modified();
}

void UltrasoundSectorSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}


//----------------------------------------------------------------------------
int UltrasoundSectorSource::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
	if (!mSector)
		return 1;

	// get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkPolyData *output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  output->SetPoints(mSector->GetPoints());
  output->GetPointData()->SetTCoords(mSector->GetPointData()->GetTCoords());
  output->SetStrips(mSector->GetStrips());

  return 1;
}

//----------------------------------------------------------------------------
int UltrasoundSectorSource::RequestInformation(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  outInfo->Set(vtkStreamingDemandDrivenPipeline::MAXIMUM_NUMBER_OF_PIECES(),
			   -1);

  double* bounds = mSector->GetBounds();
  if (mSector)
	  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_BOUNDING_BOX(),
				   bounds[0],
				   bounds[1],
				   bounds[2],
				   bounds[3],
				   bounds[4],
				   bounds[5]);

  return 1;
}
