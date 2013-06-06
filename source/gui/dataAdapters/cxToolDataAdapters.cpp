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

#include <cxToolDataAdapters.h>

#include "sscTypeConversions.h"
#include "cxToolManager.h"
#include "cxTool.h"

namespace cx
{


ActiveToolStringDataAdapter::ActiveToolStringDataAdapter()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SIGNAL(changed()));
  connect(ssc::toolManager(), SIGNAL(configured()), this, SIGNAL(changed()));
}

QString ActiveToolStringDataAdapter::getValueName() const
{
  return "Active Tool";
}
bool ActiveToolStringDataAdapter::setValue(const QString& value)
{
  ssc::ToolPtr newTool = ssc::toolManager()->getTool(value);
  if (!newTool)
	  return false;
  if(newTool == ssc::toolManager()->getDominantTool())
    return false;
  ssc::toolManager()->setDominantTool(newTool->getUid());
  return true;
}
QString ActiveToolStringDataAdapter::getValue() const
{
  if (!ssc::toolManager()->getDominantTool())
    return "";
  return qstring_cast(ssc::toolManager()->getDominantTool()->getUid());
}
QString ActiveToolStringDataAdapter::getHelp() const
{
  return "select the active (dominant) tool";
}
QStringList ActiveToolStringDataAdapter::getValueRange() const
{
  std::vector<QString> uids = ssc::toolManager()->getToolUids();
  QStringList retval;
  //retval << ""; //Don't add "no tool" choice
  for (unsigned i=0; i<uids.size(); ++i)
    retval << qstring_cast(uids[i]);
  return retval;
}
QString ActiveToolStringDataAdapter::convertInternal2Display(QString internal)
{
  ssc::ToolPtr tool = ssc::toolManager()->getTool(internal);
  if (!tool)
    return "<no tool>";
  return qstring_cast(tool->getName());
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------



/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------


ActiveProbeConfigurationStringDataAdapter::ActiveProbeConfigurationStringDataAdapter()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChanged()));
  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(dominantToolChanged())); // for debugging: if initializing a manual tool with probe properties
  connect(ssc::toolManager(), SIGNAL(trackingStarted()), this, SLOT(dominantToolChanged()));
  this->dominantToolChanged();
}

void ActiveProbeConfigurationStringDataAdapter::dominantToolChanged()
{
//	std::cout << "ActiveProbeConfigurationStringDataAdapter::dominantToolChanged() "
//		<< ToolManager::getInstance()->findFirstProbe().get() << std::endl;
	// ignore tool changes to something non-probeish.
	// This gives the user a chance to use the widget without having to show the probe.
	ssc::ToolPtr newTool = ToolManager::getInstance()->findFirstProbe();
	if (!newTool || !newTool->getProbe())
		return;

//	std::cout << " probe " << newTool->getProbe().get() << std::endl;

	if (mTool)
		disconnect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

	mTool = newTool;

	if (mTool)
		connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

	emit changed();
}

QString ActiveProbeConfigurationStringDataAdapter::getValueName() const
{
  return "Probe Config";
}

bool ActiveProbeConfigurationStringDataAdapter::setValue(const QString& value)
{
  if (!mTool)
    return false;
  mTool->getProbe()->applyNewConfigurationWithId(value);
  return true;
}

QString ActiveProbeConfigurationStringDataAdapter::getValue() const
{
  if (!mTool)
    return "";
  return mTool->getProbe()->getConfigId();
}

QString ActiveProbeConfigurationStringDataAdapter::getHelp() const
{
  return "Select a probe configuration for the active probe.";
}

QStringList ActiveProbeConfigurationStringDataAdapter::getValueRange() const
{
  if (!mTool)
    return QStringList();
  return mTool->getProbe()->getConfigIdList();
}

QString ActiveProbeConfigurationStringDataAdapter::convertInternal2Display(QString internal)
{
  if (!mTool)
    return "<no tool>";
  return mTool->getProbe()->getConfigName(internal); ///< get a name for the given configuration
}


}
