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

#ifndef CXTOOLPROPERTY_H_
#define CXTOOLPROPERTY_H_

#include "cxGuiExport.h"

#include "cxStringPropertyBase.h"
#include "cxTool.h"

namespace cx
{
typedef boost::shared_ptr<class TrackingService> TrackingServicePtr;

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/** Adapter that connects to the current active tool.
 */
class cxGui_EXPORT StringPropertyActiveTool : public StringPropertyBase
{
  Q_OBJECT
public:
  static StringPropertyBasePtr New(TrackingServicePtr trackingService) { return StringPropertyBasePtr(new StringPropertyActiveTool(trackingService)); }
  explicit StringPropertyActiveTool(TrackingServicePtr trackingService);
  virtual ~StringPropertyActiveTool() {}

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

private:
  TrackingServicePtr mTrackingService;
};


typedef boost::shared_ptr<class StringPropertyActiveProbeConfiguration> StringPropertyActiveProbeConfigurationPtr;

/** Adapter that connects to the current active probe.
 *  It will stick to the probe as much as possible,
 *  i.e. ignore hiding and showing of other non-probes.
 */
class cxGui_EXPORT StringPropertyActiveProbeConfiguration : public StringPropertyBase
{
  Q_OBJECT
public:
  static StringPropertyActiveProbeConfigurationPtr New(TrackingServicePtr trackingService) { return StringPropertyActiveProbeConfigurationPtr(new StringPropertyActiveProbeConfiguration(trackingService)); }
  explicit StringPropertyActiveProbeConfiguration(TrackingServicePtr trackingService);
  virtual ~StringPropertyActiveProbeConfiguration() {}

public: // basic methods
  virtual QString getDisplayName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);

  // extensions
  ToolPtr getTool() { return mTool; }

private slots:
  void activeToolChanged();
private:
  ToolPtr mTool;
  TrackingServicePtr mTrackingService;
};


/**
 * @}
 */
}

#endif /* CXTOOLPROPERTY_H_ */
