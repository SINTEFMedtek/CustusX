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

#include <cxDominantToolProxy.h>
#include "sscToolManager.h"

namespace cx
{

DominantToolProxy::DominantToolProxy()
{
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this,
					SLOT(dominantToolChangedSlot(const QString&)));
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this,
					SIGNAL(dominantToolChanged(const QString&)));
}

void DominantToolProxy::dominantToolChangedSlot(const QString& uid)
{
	if (mTool && mTool->getUid() == uid)
		return;

	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
						SIGNAL(toolTransformAndTimestamp(Transform3D, double)));
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));
		disconnect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(tooltipOffset(double)));
		disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
		disconnect(mTool.get(), SIGNAL(tps(int)), this, SIGNAL(tps(int)));
	}

	mTool = ssc::toolManager()->getDominantTool();

	if (mTool)
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this,
						SIGNAL(toolTransformAndTimestamp(Transform3D, double)));
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));
		connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(tooltipOffset(double)));
		connect(mTool.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
		connect(mTool.get(), SIGNAL(tps(int)), this, SIGNAL(tps(int)));
	}
}

}
