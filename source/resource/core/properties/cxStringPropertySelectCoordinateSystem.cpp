/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxStringPropertySelectCoordinateSystem.h"

#include "cxTrackingService.h"
#include "cxTypeConversions.h"
#include "cxDefinitionStrings.h"

namespace cx
{

StringPropertySelectCoordinateSystemBase::StringPropertySelectCoordinateSystemBase()
{
}

QStringList StringPropertySelectCoordinateSystemBase::getValueRange() const
{
  QStringList retval;
  retval << "";
  retval << qstring_cast(csREF);
  retval << qstring_cast(csDATA);
  retval << qstring_cast(csPATIENTREF);
  retval << qstring_cast(csTOOL);
  retval << qstring_cast(csSENSOR);
  return retval;
}

QString StringPropertySelectCoordinateSystemBase::convertInternal2Display(QString internal)
{
  if (internal.isEmpty())
	return "<no coordinate system>";

  //as requested by Frank
  if(internal == "reference")
	return "data reference";
  if(internal == "data")
	return "data (image/mesh)";
  if(internal == "patient reference")
	return "patient/tool reference";
  if(internal == "tool")
	return "tool";
  if(internal == "sensor")
	return "tools sensor";

  return internal;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertySelectCoordinateSystem::StringPropertySelectCoordinateSystem(TrackingServicePtr trackingService)
{
	mTrackingService = trackingService;
	mCoordinateSystem = csCOUNT;
  connect(trackingService.get(), &TrackingService::stateChanged, this, &StringPropertySelectCoordinateSystem::setDefaultSlot);
}

QString StringPropertySelectCoordinateSystem::getDisplayName() const
{
  return "Select coordinate system";
}

bool StringPropertySelectCoordinateSystem::setValue(const QString& value)
{
  mCoordinateSystem = string2enum<COORDINATE_SYSTEM>(value);
  emit changed();
  return true;
}

QString StringPropertySelectCoordinateSystem::getValue() const
{
  return qstring_cast(mCoordinateSystem);
}

QString StringPropertySelectCoordinateSystem::getHelp() const
{
  return "Select a coordinate system";
}

void StringPropertySelectCoordinateSystem::setDefaultSlot()
{
  this->setValue(qstring_cast(csPATIENTREF));
}

} // namespace cx
