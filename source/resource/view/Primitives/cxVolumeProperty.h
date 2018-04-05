/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXVOLUMEPROPERTY_H
#define CXVOLUMEPROPERTY_H

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"

namespace cx
{

typedef boost::shared_ptr<class VolumeProperty> VolumePropertyPtr;

/** Wrapper for vtkVolumeProperty
  * Given an internal Image, the vtkVolumeProperty
  * is kept up to date.
  *
  * \ingroup cx_resource_view
  * \date 10 Sep 2013, 2013
  * \author Christian Askeland, SINTEF
  * \author Ole Vegard Solberg, SINTEF
  */
class cxResourceVisualization_EXPORT VolumeProperty : public QObject
{
	Q_OBJECT
public:
	static VolumePropertyPtr create() { return VolumePropertyPtr(new VolumeProperty()); }
	void setImage(ImagePtr image);
//	ImagePtr getImage();
	vtkVolumePropertyPtr getVolumeProperty();

private slots:
	void transferFunctionsChangedSlot();

private:
	VolumeProperty();

	ImagePtr mImage;
	vtkVolumePropertyPtr mVolumeProperty;
	vtkPiecewiseFunctionPtr mOpacityTransferFunction;
	vtkColorTransferFunctionPtr mColorTransferFunction;
};


} // namespace cx


#endif // CXVOLUMEPROPERTY_H
