/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLPROPERTY_H_
#define CXTOOLPROPERTY_H_

#include "cxResourceExport.h"

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
class cxResource_EXPORT StringPropertyActiveTool : public StringPropertyBase
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
class cxResource_EXPORT StringPropertyActiveProbeConfiguration : public StringPropertyBase
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
