// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXVOLUMEPROPERTY_H
#define CXVOLUMEPROPERTY_H

#include <QObject>
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

namespace cx
{

typedef boost::shared_ptr<class VolumeProperty> VolumePropertyPtr;

/** Wrapper for vtkVolumeProperty
  * Given an internal Image, the vtkVolumeProperty
  * is kept up to date.
  *
  * \ingroup sscProxy
  * \date 10 Sep 2013, 2013
  * \author Christian Askeland, SINTEF
  * \author Ole Vegard Solberg, SINTEF
  */
class VolumeProperty : public QObject
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
