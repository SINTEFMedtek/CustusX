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

#include "cxDistanceMetricRep.h"
#include "cxView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include "cxTypeConversions.h"

namespace cx
{

DistanceMetricRepPtr DistanceMetricRep::New(const QString& uid, const QString& name)
{
	DistanceMetricRepPtr retval(new DistanceMetricRep(uid, name));
	return retval;
}

DistanceMetricRep::DistanceMetricRep(const QString& uid, const QString& name) :
                DataMetricRep(uid, name)
{
}

void DistanceMetricRep::clear()
{
    DataMetricRep::clear();
    mGraphicalLine.reset();
}

DistanceMetricPtr DistanceMetricRep::getDistanceMetric()
{
    return boost::dynamic_pointer_cast<DistanceMetric>(mMetric);
}

void DistanceMetricRep::onModifiedStartRender()
{
//	std::cout << "  changedslot" << this << std::endl;
	DistanceMetricPtr distanceMetric = this->getDistanceMetric();
	if (!distanceMetric)
		return;

	if (!mMetric->isValid())
		return;

	if (!mGraphicalLine && mView)
	{
		mGraphicalLine.reset(new GraphicalLine3D(mView->getRenderer()));
	}

	if (mGraphicalLine)
	{
		mGraphicalLine->setColor(mMetric->getColor());
		std::vector<Vector3D> p = distanceMetric->getEndpoints();
		mGraphicalLine->setValue(p[0], p[1]);
		mGraphicalLine->setStipple(0xF0FF);
	}

	this->drawText();
}


}
