/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
