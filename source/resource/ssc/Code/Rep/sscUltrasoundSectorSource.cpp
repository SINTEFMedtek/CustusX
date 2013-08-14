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

#include "sscUltrasoundSectorSource.h"

#include "vtkObjectFactory.h"
#include <vtkPointData.h>

vtkCxxRevisionMacro(UltrasoundSectorSource, "$Revision: 3.0 $")
;
vtkStandardNewMacro(UltrasoundSectorSource)
;

UltrasoundSectorSource::UltrasoundSectorSource()
{
}

void UltrasoundSectorSource::Execute()
{
	if (!mSector)
		return;

	vtkPolyData *output = this->GetOutput();

	output->SetPoints(mSector->GetPoints());
	output->GetPointData()->SetTCoords(mSector->GetPointData()->GetTCoords());
	//output->SetLines(mSector->GetLines());
	//output->SetPolys(mSector->GetPolys());
	output->SetStrips(mSector->GetStrips());
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
