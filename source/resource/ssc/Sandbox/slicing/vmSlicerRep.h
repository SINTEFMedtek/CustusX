//
// C++ Interface: vmSlicerRep
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef VMSLICERREP_H
#define VMSLICERREP_H

#include <vector>

#include "vmForwardDeclarations.h"
#include "Utilities.h"
#include "snwVtkIncludes.h"

class vtkScalarsToColors;

namespace vm
{

class SlicerRep
{
	public:
		SlicerRep ( vtkLookupTablePtr LookupTable, const VolumePtr& data );
		~SlicerRep();

		vtkImageActorPtr getActor();
		void updateMatrixPosition ( const Vector3D& center );
		void setMatrixData ( const Vector3D& ivec, const Vector3D& jvec, const Vector3D& center );

	private:
		void setImageData ( const VolumePtr& data );
		void setLookupTable ( vtkScalarsToColors *lut );
		void create();

		VolumePtr mData;
		vtkImageMapToWindowLevelColorsPtr mWindowLevel;
		vtkImageReslicePtr mReslicer;
		vtkMatrix4x4Ptr mMatrixAxes;
		vtkImageActorPtr mImageActor;
};

}

#endif
