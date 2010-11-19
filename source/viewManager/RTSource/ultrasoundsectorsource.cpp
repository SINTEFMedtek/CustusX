#include "ultrasoundsectorsource.h"

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

/*
 * $Log: ultrasoundsectorsource.cpp,v $
 * Revision 1.1  2004/10/12 06:32:23  geirat
 * File created
 *
 */

