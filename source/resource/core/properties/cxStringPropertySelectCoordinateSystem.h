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
#ifndef CXSTRINGPROPERTYSELECTCOORDINATESYSTEM_H
#define CXSTRINGPROPERTYSELECTCOORDINATESYSTEM_H

#include "cxResourceExport.h"

#include "cxStringPropertyBase.h"
#include "cxForwardDeclarations.h"
#include "cxCoordinateSystemHelpers.h"

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

namespace cx
{

/** Base class for all Properties that selects a coordinatesystem.
 */
class cxResource_EXPORT StringPropertySelectCoordinateSystemBase : public StringPropertyBase
{
  Q_OBJECT
public:
  StringPropertySelectCoordinateSystemBase();
  virtual ~StringPropertySelectCoordinateSystemBase() {}

public: // basic methods

public: // optional methods
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};
typedef boost::shared_ptr<class StringPropertySelectCoordinateSystemBase> StringPropertySelectCoordinateSystemBasePtr;


typedef boost::shared_ptr<class StringPropertySelectCoordinateSystem> StringPropertySelectCoordinateSystemPtr;

/**
 * \brief Adapter that selects and stores a coordinate systems.
 * The coordinatesystem is stored internally in the adapter.
 * Use setValue/getValue plus changed() to access it.
 *
 * Class reacts to toolmanagers configurerd signal and automatically sets patientref as default
 */
class cxResource_EXPORT StringPropertySelectCoordinateSystem : public StringPropertySelectCoordinateSystemBase
{
  Q_OBJECT
public:
  static StringPropertySelectCoordinateSystemPtr New(TrackingServicePtr trackingService) { return StringPropertySelectCoordinateSystemPtr(new StringPropertySelectCoordinateSystem(trackingService)); }
  StringPropertySelectCoordinateSystem(TrackingServicePtr trackingService);
  virtual ~StringPropertySelectCoordinateSystem() {}

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;

public: // interface extension
  COORDINATE_SYSTEM getCoordinateSystem();

private slots: //interface extension
  void setDefaultSlot();

private:
  COORDINATE_SYSTEM mCoordinateSystem;
  QString mValueName;
  TrackingServicePtr mTrackingService;
};


} // namespace cx


/**
 * @}
 */

#endif // CXSTRINGPROPERTYSELECTCOORDINATESYSTEM_H
