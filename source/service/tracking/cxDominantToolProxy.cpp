/*
 * cxDominantToolProxy.cpp
 *
 *  Created on: Sep 9, 2011
 *      Author: christiana
 */

#include <cxDominantToolProxy.h>
#include "sscToolManager.h"

namespace cx
{

DominantToolProxy::DominantToolProxy()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot(const QString&)));
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SIGNAL(dominantToolChanged(const QString&)));
}

void DominantToolProxy::dominantToolChangedSlot(const QString& uid)
{
  if(mTool && mTool->getUid() == uid)
    return;

  if(mTool)
  {
    disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SIGNAL(toolTransformAndTimestamp(Transform3D, double)));
    disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));
    disconnect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(tooltipOffset(double)));
    disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
    disconnect(mTool.get(), SIGNAL(tps(int)), this, SIGNAL(tps(int)));
    disconnect(mTool.get(), SIGNAL(), this, SIGNAL());
  }

  mTool = ssc::toolManager()->getDominantTool();

  if(mTool)
  {
    connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SIGNAL(toolTransformAndTimestamp(Transform3D, double)));
    connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));
    connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(tooltipOffset(double)));
    connect(mTool.get(), SIGNAL(toolProbeSector()), this, SIGNAL(toolProbeSector()));
    connect(mTool.get(), SIGNAL(tps(int)), this, SIGNAL(tps(int)));
    connect(mTool.get(), SIGNAL(), this, SIGNAL());
  }
}

}
