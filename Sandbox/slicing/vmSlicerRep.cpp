//
// C++ Implementation: vmSlicerRep
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "vmSlicerRep.h"

#include "StdIncludes.h"
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkActor2D.h>
#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkRenderer.h>
#include <vtkLookupTable.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkProperty2D.h>

#include "vmVolume.h"

//---------------------------------------------------------
namespace vm
{
//---------------------------------------------------------

SlicerRep::SlicerRep ( vtkLookupTablePtr LookupTable, const VolumePtr& data )
{
	mImageActor = vtkImageActorPtr::New();
	mMatrixAxes = vtkMatrix4x4Ptr::New();
	mReslicer = vtkImageReslicePtr::New();
	mWindowLevel = vtkImageMapToWindowLevelColorsPtr::New();
	
	setLookupTable ( LookupTable );
	setImageData ( data );
	create(); //create the slice
}

SlicerRep::~SlicerRep()
{
}

void SlicerRep::setImageData ( const VolumePtr& data )
{
	mData = data;
	if (mData)
	{
		mData->updateInformation();
	}
}

vtkImageActorPtr SlicerRep::getActor()
{
	return mImageActor;
}

void SlicerRep::create()
{
	if (!mData)
	{
		return;
	}
	mReslicer->SetInput ( mData->getVtkImageData() );
	mReslicer->SetInterpolationModeToLinear();

	mReslicer->GetOutput()->UpdateInformation();
	mReslicer->SetOutputDimensionality ( 2 );
	mReslicer->SetResliceAxes ( mMatrixAxes ) ;
	mReslicer->SetAutoCropOutputOn();

	mWindowLevel->SetInputConnection ( mReslicer->GetOutputPort() );
	mImageActor->SetInput ( mWindowLevel->GetOutput() );
}

void SlicerRep::updateMatrixPosition ( const Vector3D& center )
{
	 SW_LOG("Update Matrix position" );

	for ( unsigned i = 0; i < 3; ++i )
	{
		mMatrixAxes->SetElement ( i, 3, center[i] );
	}
}

/**update a vtkMatrix4x4 representing a slice.
 * The slice is defined with its two spanning (i,j) vectors
 * and a center position.
 */
void SlicerRep::setMatrixData ( const Vector3D& ivec, const Vector3D& jvec, const Vector3D& center )
{
	Vector3D kvec = cross ( ivec, jvec );
	// set all column vectors
	mMatrixAxes->Identity();
	for ( unsigned i = 0; i < 3; ++i )
	{
		mMatrixAxes->SetElement ( i, 0, ivec[i] );
		mMatrixAxes->SetElement ( i, 1, jvec[i] );
		mMatrixAxes->SetElement ( i, 2, kvec[i] );
		mMatrixAxes->SetElement ( i, 3, center[i] );
	}
}


void SlicerRep::setLookupTable ( vtkScalarsToColors *lut )
{
	vtkLookupTable* realLut =  vtkLookupTable::SafeDownCast ( lut );
	if ( !realLut )
	{
		SW_LOG ( "Error: Cannot cast vtkScalarsToColors to vtkLookupTable." );
		return;
	}
	//ikke sikker på om dette er nødvendig
	vtkLookupTable* newLut = vtkLookupTable::New();
	newLut->DeepCopy ( realLut );
	mWindowLevel->SetLookupTable ( newLut );
	mWindowLevel->SetWindow ( 255.0 );
	mWindowLevel->SetLevel ( 127.5 );
	newLut->Delete();
}

//---------------------------------------------------------
} //namespace vm
//---------------------------------------------------------
