/*
 * cxToolDataAdapters.cpp
 *
 *  \date May 4, 2011
 *      \author christiana
 */

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


ActiveToolConfigurationStringDataAdapter::ActiveToolConfigurationStringDataAdapter()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChanged()));
}
void ActiveToolConfigurationStringDataAdapter::dominantToolChanged()
{
  // ignore tool changes to something non-probeish.
  // This gives the user a chance to use the widget without having to show the probe.
  ToolPtr newTool = boost::shared_dynamic_cast<Tool>(ssc::toolManager()->getDominantTool());
  if (!newTool || newTool->getProbeSector().mType==ssc::ProbeData::tNONE)
    return;

  if (mTool)
  	disconnect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

  mTool = newTool;

  if (mTool)
  	connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

  emit changed();
}
QString ActiveToolConfigurationStringDataAdapter::getValueName() const
{
  return "Probe Config";
}
bool ActiveToolConfigurationStringDataAdapter::setValue(const QString& value)
{
  if (!mTool)
    return false;
  mTool->getProbe()->setConfigId(value);
  return true;
}
QString ActiveToolConfigurationStringDataAdapter::getValue() const
{
  if (!mTool)
    return "";
  return mTool->getProbe()->getConfigId();
}
QString ActiveToolConfigurationStringDataAdapter::getHelp() const
{
  return "Select a probe configuration for the active tool.";
}
QStringList ActiveToolConfigurationStringDataAdapter::getValueRange() const
{
  if (!mTool)
    return QStringList();
  return mTool->getProbe()->getConfigIdList();
}
QString ActiveToolConfigurationStringDataAdapter::convertInternal2Display(QString internal)
{
  if (!mTool)
    return "<no tool>";
  return mTool->getProbe()->getConfigName(internal); ///< get a name for the given configuration
}


}
